#include "log.h"
#include "file.h"

int main()
{
    try
    {
        file f("file.txt");
        f.write("111", 0, 3);
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
