#include "arg_parser.h"
#include <algorithm>
#include <stdexcept>
#include "err_type.h"
#include "include/cpp_assert.h"

arg_parser::arg_parser(const int argc, char ** argv, const parameter_vector & parameters)
{
    // sanity check
    for (const auto & p : parameters)
    {
        assert_short(!p.name.empty());
    }

    auto contains = [&parameters](const std::string & name)->bool
    {
        return std::ranges::any_of(parameters, [&name](const parameter_t & p)->bool
        {
            return (p.name == name || (name.size() == 1 && p.short_name == name[0]));
        });
    };

    auto get_short_name = [](const std::string & name)->char
    {
        if (name.size() == 2 && name[0] == '-' && name[1] != '-')
        {
            return name[1];
        }

        return '\0';
    };

    auto get_long_name = [](const std::string & name)->std::string
    {
        if (name.size() > 2 && name[0] == '-' && name[1] == '-')
        {
            return name.substr(2);
        }

        return "";
    };

    auto is_param = [](const std::string & name)->bool
    {
        return name.size() > 1 && name[0] == '-';
    };

    auto find = [&parameters](const std::string & name)->parameter_t
    {
        const auto it =
            std::ranges::find_if(parameters, [&name](const parameter_t & p)->bool
            {
                return (p.name == name || (name.size() == 1 && p.short_name == name[0]));
            });
        if (it == parameters.end())
        {
            throw std::runtime_error("Unknown parameter: " + name);
        }

        return *it;
    };

    std::string current_arg;
    std::vector<std::string> bare;
    for (int i = 1; i < argc; ++i)
    {
        if (const std::string arg = argv[i];
            is_param(arg) && bare.empty())
        {
            const char short_name = get_short_name(arg);
            const std::string long_name = get_long_name(arg);
            const std::string name = short_name != '\0' ? std::string(1, short_name) : long_name;
            assert_throw(contains(name), "Unknown parameter: " + arg);
            const auto param_info = find(name);
            if (param_info.arg_required) {
                current_arg = param_info.name;
            } else {
                args.emplace_back(param_info.name, "");
            }
        }
        else if (current_arg.empty())
        {
            bare.push_back(arg);
        }
        else
        {
            args.emplace_back(current_arg, arg);
            current_arg.clear();
        }
    }

    for (const auto & arg : bare)
    {
        args.emplace_back("", arg);
    }
}
