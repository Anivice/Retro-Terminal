#include "log.h"
#include "file.h"
#include "arg_parser.h"
#include "color.h"

const arg_parser::parameter_vector Arguments = {
    { .name = "help", .short_name = 'h', .arg_required = false, .description = "Prints this help message" },
    { .name = "version", .short_name = 'v', .arg_required = false, .description = "Prints version" },
};

void print_help(const std::string & program_name)
{
    uint64_t max_name_len = 0;
    std::vector< std::pair <std::string, std::string>> output;
    const std::string required_str = " arg";
    for (const auto & [name, short_name, arg_required, description] : Arguments)
    {
        std::string name_str = "--" + name
            += (short_name == '\0' ? "" : ",-" + std::string(1, short_name))
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

int main(int argc, char **argv)
{
    try
    {
        // first, iterate through helpers
        for (arg_parser args(argc, argv, Arguments);
            const auto & [arg, val] : args)
        {
            if (arg == "help")
            {
                print_help(argv[0]);
                return EXIT_SUCCESS;
            }

            if (arg == "version")
            {
                std::cout << color(5,5,5) << argv[0] << no_color()
                          << color(0,3,3) << " version " << color(0,5,5) << VERSION
                          << color(3,0,3) << " built on " << color(5,0,5) << BUILD_DATE
                          << no_color() << std::endl;
                return EXIT_SUCCESS;
            }
        }
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
