#include "log.h"
#include "arg_parser.h"
#include "color.h"
#include "configuration.h"
#include "cpp_assert.h"
#include <atomic>
#include <algorithm>

std::atomic_bool g_verbose{false};

const arg_parser::parameter_vector Arguments = {
    { .name = "help", .short_name = 'h', .arg_required = false, .description = "Prints this help message" },
    { .name = "version", .short_name = 'v', .arg_required = false, .description = "Prints version" },
    { .name = "config", .short_name = 'c', .arg_required = true, .description = "Path to config file" }
};

void print_help(const std::string & program_name)
{
    uint64_t max_name_len = 0;
    std::vector< std::pair <std::string, std::string>> output;
    const std::string required_str = " arg";
    for (const auto & [name, short_name, arg_required, description] : Arguments)
    {
        std::string name_str =
            (short_name == '\0' ? "" : "-" + std::string(1, short_name))
            += ",--" + name
            += (arg_required ? required_str : "");

        if (max_name_len < name_str.size())
        {
            max_name_len = name_str.size();
        }

        output.emplace_back(name_str, description);
    }

    std::cout << color(5,5,5) << program_name << no_color() << color(0,2,5) << " [options]" << no_color()
              << std::endl << color(1,2,3) << "options:" << no_color() << std::endl;
    for (const auto & [name, description] : output)
    {
        std::cout << "    " << color(1,5,4) << name << no_color()
                  << std::string(max_name_len + 4 - name.size(), ' ')
                  << color(4,5,1) << description << no_color() << std::endl;
    }
}

#define assert_one_value(value, key) (assert_throw((value).size() == 1, "Multiple definition or simply lack of a valid definition for `" key "`"))

void debug_section_config(const std::map < std::string, std::vector<std::string> > & key_pairs)
{
    for (const auto & [key, value] : key_pairs)
    {
        if (key == "symbol_table")
        {
            assert_one_value(value, "symbol_table");
            if (backtrace_level_1_init_.symbol_vector.empty()) {
                backtrace_level_1_init_.initialize(value.front().c_str());
            }

            if (DEBUG) debug_log("Symbol table loaded from file ", value.front(), "\n");
        } else if (key == "backtrace_level") {
            pre_defined_level = static_cast<int>(std::strtol(value.front().c_str(), nullptr, 10));
        } else if (key == "verbose") {
            g_verbose = true_false_helper(value.front());
            if (g_verbose) { debug_log("Verbose mode enabled\n"); }
        } else if (key == "trim_symbol") {
            trim_symbol = true_false_helper(value.front());
        } else {
            if (DEBUG) debug_log(color(5, 5, 0), "WARNING: `", key, "` is not a valid key name, ignored\n", no_color());
        }
    }
}

#undef assert_one_value

void process_config(const configuration & config)
{
    for (const auto & [section, vector] : config)
    {
        if (section == "debug") {
            debug_section_config(vector);
        }
    }
}

int main(int argc, char **argv)
{
    try
    {
        arg_parser args(argc, argv, Arguments);
        auto contains = [&args](const std::string & name, std::string & val)->bool
        {
            const auto it = std::ranges::find_if(args,
                [&name](const std::pair<std::string, std::string> & p)->bool{ return p.first == name; });
            if (it != args.end())
            {
                val = it->second;
                return true;
            }

            return false;
        };

        for (const auto & [arg, val] : args)
        {
            debug_log(arg, " ", val, "\n");
        }

        std::string arg_val;
        if (contains("help", arg_val)) // GNU compliance, help must be processed first if it appears and ignore all other arguments
        {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        }

        if (contains("version", arg_val))
        {
            std::cout << color(5,5,5) << argv[0] << no_color()
                    << color(0,3,3) << " version " << color(0,5,5) << VERSION
                    << no_color() << std::endl;
            return EXIT_SUCCESS;
        }

        if (contains("config", arg_val))
        {
            process_config(configuration(arg_val));
        }

        assert_short(false);
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
