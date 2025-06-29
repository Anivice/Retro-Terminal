#include <iostream>
#include <iomanip>
#include "test/test.h"
#include "helper/color.h"

extern void show();

int main()
{
    std::cout << "\x1b[?25l"; // hide cursor
    int current_test = 1;
    const auto all_tests = test::unit_tests.size();
    int success = 0;
    int failed = 0;
    for (const auto unit_address : test::unit_tests)
    {
        auto * base_unit = (test::unit_t *)unit_address;
        std::cout << "\x1b[2K\r" << color::color(1,5,4) << "TEST\t[" << current_test++ << "/" << all_tests << "]\t";
        if (base_unit->run()) {
            std::cout << color::color(0,5,0) << "PASSED " << color::color(0,2,2) << base_unit->success() << "\r";
            success++;
        } else {
            std::cout << color::color(5,0,0) << "FAILED " << base_unit->failure() << "\n";
            failed++;
        }

        std::cout.flush();
    }

    std::cout << "\n\r\n"
              << color::color(1,5,4) << all_tests << " UNIT TESTS\n"
              << color::color(0,5,0) << "    " << success << " PASSED\n"
              << color::color(5,0,0) << "    " << failed << " FAILED (2 DESIGNED TO FAIL)\n" << color::no_color();
    std::cout << "\x1b[?25h" << std::endl; // show cursor

    if (failed == 2)
    {
        std::cout << color::color(0,5,0)
                  << "Congratulations, all unit tests passed (with 2 designed to fail)"
                  << color::no_color() << std::endl;
        show();
        std::cout << std::endl;
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
