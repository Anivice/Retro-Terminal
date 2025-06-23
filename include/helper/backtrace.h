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

extern backtrace_level_1_init g_backtrace_level_1_init_;
extern std::atomic_int g_pre_defined_level;
extern std::atomic_bool g_trim_symbol;
bool true_false_helper(std::string val);

#endif //BACKTRACE_H
