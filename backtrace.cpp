#include "backtrace.h"
#include "color.h"
#include "get_env.h"
#include <atomic>
#include <vector>
#include <execinfo.h>
#include <sstream>
#include <regex>
#include "execute.h"
#define MAX_STACK_FRAMES (64)

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

            ss  << color(0,4,1) << "Frame " << color(5,0,0) << "#" << i++ << " "
                << std::hex << color(2,4,5) << frame
                << ": " << color(1,5,5) << info.name << no_color << "\n";
            if (!info.file.empty())
                ss << "          " << color(0,1,5) << info.file << no_color << "\n";
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
