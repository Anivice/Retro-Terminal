#ifndef BACKTRACE_H
#define BACKTRACE_H

#include <string>
#include <vector>
#include <cstdint>
#include <atomic>

std::string backtrace();

class backtrace_level_1_init
{
public:
    std::vector<std::pair<uint64_t, std::string>> symbol_vector{};
    void initialize(const char * path);
    backtrace_level_1_init();
};

extern backtrace_level_1_init backtrace_level_1_init_;
extern std::atomic_int pre_defined_level;
extern std::atomic_bool trim_symbol;
bool true_false_helper(std::string val);

#endif //BACKTRACE_H
