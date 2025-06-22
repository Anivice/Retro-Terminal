#include "get_env.h"

std::string get_env(const std::string & name)
{
    const char * ptr = std::getenv(name.c_str());
    if (ptr == nullptr)
    {
        return "";
    }

    return ptr;
}
