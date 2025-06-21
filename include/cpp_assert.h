#ifndef CPP_ASSERT_H
#define CPP_ASSERT_H

#include "err_type.h"

inline void assert_throw(const bool condition, const std::string & message)
{
    if (!condition)
    {
        throw runtime_error(message);
    }
}

#endif //CPP_ASSERT_H
