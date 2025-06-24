#ifndef BACKTRACE_H
#define BACKTRACE_H

#include <string>
#include <atomic>

std::string backtrace();
extern std::atomic_int g_pre_defined_level;
extern std::atomic_bool g_trim_symbol;
bool true_false_helper(std::string val);

#endif //BACKTRACE_H
