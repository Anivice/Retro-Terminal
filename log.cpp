#include "log.h"

std::mutex debug::log_mutex;
char debug::split = 0;

#if DEBUG
bool debug::do_i_show_caller_next_time = true;
#endif
