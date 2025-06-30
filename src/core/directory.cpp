#include "core/directory.h"
#include "helper/base64.hpp"

directory_t::entry_t directory_t::path_to_entry(const std::string& path)
{
    return base64::to_base64(path);
}
