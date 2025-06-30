#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

#include <array>
#include <stdexcept>
#include "core/directory.h"

class no_such_block final : public std::runtime_error { public: no_such_block() : std::runtime_error("No such block") { } };
directory_t::block_t get_block_on_my_end(const std::string & hash);
std::string write_block_on_my_end(const directory_t::block_t & block);

#endif //FILE_ACCESS_H
