#include "get_env.h"
#include <cstdlib>

std::string get_env(const std::string & name)
{
    const char * ptr = secure_getenv(name.c_str());
    if (ptr == nullptr)
    {
        return "";
    }

    return ptr;
}
