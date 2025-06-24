#include "helper/log.h"

std::mutex debug::log_mutex;

#ifndef __NO_CALLER__
bool debug::do_i_show_caller_next_time = true;
std::string strip_name(const std::string & name)
{
    const std::regex pattern(R"([\w]+ (.*)\(.*\))");
    if (std::smatch matches; std::regex_match(name, matches, pattern) && matches.size() > 1) {
        return matches[1];
    }

    return name;
}
#endif
