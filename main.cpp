#include "log.h"

int main()
{
    debug_log(std::string("Hello World!"));
    debug_log("Hello World!\n");
    debug_log(std::string_view("Hello World!\n"));
    debug_log("log::split=' '");
    debug_log("H", 'A');
}
