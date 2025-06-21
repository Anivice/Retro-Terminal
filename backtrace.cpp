#include "backtrace.h"

#include <atomic>
#include <vector>
#include <execinfo.h>
#include <sstream>
#include <regex>
#include "execute.h"
#define MAX_STACK_FRAMES (64)
// env variables
#define BACKTRACE_LEVEL "BACKTRACE_LEVEL"
#define COLOR "COLOR"

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

std::string get_env(const std::string & name)
{
    const char * ptr = std::getenv(name.c_str());
    if (ptr == nullptr)
    {
        return "";
    }

    return ptr;
}

template <typename IntType>
IntType get_variable(const std::string & name)
{
    const auto var = get_env(name);
    if (var.empty())
    {
        return 0;
    }

    return static_cast<IntType>(strtoll(var.c_str(), nullptr, 10));
}

std::string color(int r, int g, int b, int br, int bg, int bb)
{
    const auto color_env = get_env(COLOR);
    if (color_env == "never" || color_env == "none" || color_env == "off" || color_env == "no" || color_env == "n")
    {
        return "";
    }

    auto constrain = [](int var, int min, int max)->int
    {
        var = std::max(var, min);
        var = std::min(var, max);
        return var;
    };

    r = constrain(r, 0, 5);
    g = constrain(g, 0, 5);
    b = constrain(b, 0, 5);

    br = constrain(br, 0, 5);
    bg = constrain(bg, 0, 5);
    bb = constrain(bb, 0, 5);

    int scale = 16 + 36 * r + 6 * g + b;
    int bg_scale = 16 + 36 * br + 6 * bg + bb;

    return "\033[38;5;" + std::to_string(scale) + "m" + "\033[48;5;" + std::to_string(bg_scale) + "m";
}

std::atomic < const char * > no_color = "\033[0m";

std::string backtrace_level_1()
{
    return "BACKTRACE LEVEL 1";
}

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

        if (const size_t pos2 = caller.find('('); pos2 != std::string::npos) {
            caller = caller.substr(0, pos2);
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

            ss  << color(0,4,1,0,0,0) << "Frame " << color(5,0,0,0,0,2) << "#" << i++ << " "
                << std::hex << color(2,4,5, 0, 0, 0) << frame
                << ": " << color(1,5,5,0,0,0) << info.name << no_color << "\n";
            if (!info.file.empty())
                ss << "          " << color(1,1,3, 0, 0, 0) << info.file << no_color << "\n";
        } else {
            ss << "No trace information for " << std::hex << frame << "\n";
        }
    }

    return ss.str();
}

std::string backtrace()
{
    switch (const auto level = get_variable<int>(BACKTRACE_LEVEL))
    {
        case 1: return backtrace_level_1();
        case 2: return backtrace_level_2({"backtrace.cpp", "err_type.h"});
        default: return "";
    }
}
