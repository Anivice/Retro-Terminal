#include "helper/backtrace.h"
#include "helper/color.h"
#include "helper/get_env.h"
#include <atomic>
#include <vector>
#include <execinfo.h>
#include <sstream>
#include <regex>
#include "helper/execute.h"
#include <dlfcn.h>
#include "helper/log.h"
#include <cxxabi.h>
#include <algorithm>
#include <ranges>
#define MAX_STACK_FRAMES (64)

std::string demangle(const char* mangled)
{
    int status = 0;
    char* dem = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
    std::string result = (status == 0 && dem) ? dem : mangled;
    std::free(dem);
    return result;
}

typedef std::vector < std::pair<std::string, void*> > backtrace_info;
backtrace_info obtain_stack_frame()
{
    backtrace_info result;
    void* buffer[MAX_STACK_FRAMES] = {};
    const int frames = backtrace(buffer, MAX_STACK_FRAMES);

    char** symbols = backtrace_symbols(buffer, frames);
    if (symbols == nullptr) {
        return backtrace_info {};
    }

    for (int i = 1; i < frames; ++i) {
        result.emplace_back(symbols[i], buffer[i]);
    }

    free(symbols);
    return result;
}

extern "C" void landmark(void)
{
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
}

void backtrace_level_1_init::initialize(const char * path)
{
    void *handle = dlopen(path, RTLD_LAZY);
    if (!handle)
    {
        return;
    }

    auto * sym_map_len = static_cast<unsigned int*>(dlsym(handle, "sym_map_len"));
    auto * sym_map = static_cast<unsigned char*>(dlsym(handle, "sym_map"));

    std::vector<char> sym_map_data;
    sym_map_data.resize(*sym_map_len + 1);
    std::memcpy(sym_map_data.data(), sym_map, *sym_map_len);
    sym_map_data[*sym_map_len] = 0;
    const std::string sym_map_str(sym_map_data.data());
    std::stringstream ss(sym_map_str);
    uint64_t landmark_addr = 0;
    while (ss)
    {
        std::string hex_addr, symbol;
        ss >> hex_addr >> symbol;
        if (hex_addr.empty() || symbol.empty())
        {
            break;
        }

        symbol_vector.emplace_back(std::make_pair<uint64_t, std::string>(
            std::stoul(hex_addr, nullptr, 16),
            demangle(symbol.c_str())));
        if (symbol == "landmark")
        {
            landmark_addr = std::stoul(hex_addr, nullptr, 16);
        }
    }

    std::ranges::sort(symbol_vector,
                      [](const std::pair<uint64_t, std::string> & a, const std::pair<uint64_t, std::string> & b)->bool
                      {
                          return a.first < b.first;
                      });

    const int64_t offset = (uint64_t)(void*)landmark - landmark_addr;
    for (auto& addr : symbol_vector | std::views::keys)
    {
        addr += offset;
    }
}

backtrace_level_1_init::backtrace_level_1_init()
{
    const auto sym_map_lib = get_env("SYMBOL_MAP");
    if (sym_map_lib.empty())
    {
        return;
    }

    initialize(sym_map_lib.c_str());
}

backtrace_level_1_init g_backtrace_level_1_init_;
std::atomic_bool g_trim_symbol = false;

std::string replace_all(
    std::string & original,
    const std::string & target,
    const std::string & replacement)
{
    if (target.empty()) return original; // Avoid infinite loop if target is empty

    if (target.size() == 1 && replacement.empty()) {
        std::erase_if(original, [&target](const char c) { return c == target[0]; });
        return original;
    }

    size_t pos = 0;
    while ((pos = original.find(target, pos)) != std::string::npos) {
        original.replace(pos, target.length(), replacement);
        pos += replacement.length(); // Move past the replacement to avoid infinite loop
    }
    return original;
}

bool trim_symbol_yes()
{
    return get_env("TRIM_SYMBOL").empty() ? g_trim_symbol.load() : true_false_helper(get_env("TRIM_SYMBOL"));
}

bool true_false_helper(std::string val)
{
    std::ranges::transform(val, val.begin(), ::tolower);
    if (val == "true") {
        return true;
    } else if (val == "false") {
        return false;
    } else {
        return std::stoi(val) != 0;
    }
}

// fast backtrace
std::string backtrace_level_1()
{
    if (g_backtrace_level_1_init_.symbol_vector.empty())
    {
        return "BACKTRACE LEVEL 1: No symbol map";
    }
    std::stringstream ss;
    const backtrace_info frames = obtain_stack_frame();
    int i = 0;
    auto trim_sym = [](std::string name)->std::string
    {
        if (trim_symbol_yes())
        {
            name = std::regex_replace(name, std::regex(R"(\(.*\))"), "");
            name = std::regex_replace(name, std::regex(R"(\[abi\:.*\])"), "");
            name = std::regex_replace(name, std::regex(R"(std\:\:.*\:\:)"), "");
            return name;
        }

        return name;
    };

    for (auto & [symbol, frame] : frames)
    {
        std::string symbol_name;
        for (auto & [addr, symbol_in_map] : g_backtrace_level_1_init_.symbol_vector)
        {
            if (addr > (uint64_t)frame)
            {
                break;
            }

            symbol_name = symbol_in_map;
        }

        ss  << color(0,4,1) << "Frame " << color(5,2,1) << "#" << i++ << " "
            << std::hex << color(2,4,5) << frame
            << ": " << color(1,5,5) << trim_sym(symbol_name) << no_color() << "\n";
    }

    return ss.str();
}

// slow backtrace, with better trace info
std::string backtrace_level_2(const std::vector<std::string> & excluded_file_list)
{
    std:: stringstream ss;
    const auto frames = obtain_stack_frame();
    int i = 0;
    const std::regex pattern(R"(([^\(]+)\(([^\)]*)\) \[([^\]]+)\])");
    std::smatch matches;

    struct traced_info
    {
        std::string name;
        std::string file;
    };

    auto generate_addr2line_trace_info = [](const std::string & executable_path, const std::string& address)->traced_info
    {
        auto [fd_stdout, fd_stderr, exit_status]
            = exec_command("/usr/bin/addr2line", "", "--demangle", "-f", "-p", "-a", "-e",
                executable_path, address);

        if (exit_status != 0)
        {
            return {};
        }

        std::string caller, path;
        if (const size_t pos = fd_stdout.find('/'); pos != std::string::npos) {
            caller = fd_stdout.substr(0, pos - 4);
            path = fd_stdout.substr(pos);
        }

        if (trim_symbol_yes())
        {
            if (const size_t pos2 = caller.find('('); pos2 != std::string::npos) {
                caller = caller.substr(0, pos2);
            }
        }

        if (!caller.empty() && !path.empty()) {
            return {.name = caller, .file = replace_all(path, "\n", "") };
        }

        return {.name = replace_all(fd_stdout, "\n", ""), .file = ""};
    };

    for (const auto & [symbol, frame] : frames)
    {
        if (std::regex_search(symbol, matches, pattern) && matches.size() > 3)
        {
            const std::string& executable_path = matches[1].str();
            const std::string& traced_address = matches[2].str();
            const std::string& traced_runtime_address = matches[3].str();
            traced_info info;
            if (traced_address.empty()) {
                info = generate_addr2line_trace_info(executable_path, traced_runtime_address);
            } else {

                info = generate_addr2line_trace_info(executable_path, traced_address);
            }

            if (!info.file.empty())
            {
                bool skip = false;
                for (const auto & excluded_file : excluded_file_list)
                {
                    if (info.file.find(excluded_file) != std::string::npos)
                    {
                        skip = true;
                        break;
                    }
                }

                if (skip)
                {
                    continue;
                }
            }

            ss  << color(0,4,1) << "Frame " << color(5,2,1) << "#" << i++ << " "
                << std::hex << color(2,4,5) << frame
                << ": " << color(1,5,5) << info.name << no_color() << "\n";
            if (!info.file.empty())
                ss << "          " << color(0,1,5) << info.file << no_color() << "\n";
        } else {
            ss << "No trace information for " << std::hex << frame << "\n";
        }
    }

    return ss.str();
}

std::atomic_int g_pre_defined_level = -1;

std::string backtrace()
{
    switch (/* const auto level = */get_env(BACKTRACE_LEVEL).empty() ? g_pre_defined_level.load() : get_variable<int>(BACKTRACE_LEVEL))
    {
        case 1: return backtrace_level_1();
        case 2: return backtrace_level_2({"backtrace.cpp", "err_type.h"});
        default: return "";
    }
}
