#ifndef ERR_TYPE_H
#define ERR_TYPE_H

#include <stdexcept>
#include "backtrace.h"

class runtime_error final : public std::runtime_error
{
    std::string additional;
public:
    explicit runtime_error(const std::string& what_arg) : std::runtime_error(what_arg)
    {
        additional = what_arg;
        additional += "\n";
        additional += backtrace();
    }

    [[nodiscard]] const char* what() const noexcept override
    {
        return additional.c_str();
    }
};

#endif //ERR_TYPE_H
