#ifndef GET_ENV_H
#define GET_ENV_H

// env variables
#define BACKTRACE_LEVEL "BACKTRACE_LEVEL"
#define COLOR "COLOR"

#include <string>

std::string get_env(const std::string & name);

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

#endif //GET_ENV_H
