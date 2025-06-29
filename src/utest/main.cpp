#if DEBUG
#include <iostream>
#include <iomanip>
#include <thread>
#include <sstream>
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
        constexpr int length = 64;
        std::stringstream output_head;
        output_head << "\x1b[2K\r" << color::color(1,5,4) << "TEST\t[" << current_test++ << "/" << all_tests << "]\t"
                  << base_unit->name() << std::string(std::max(length - static_cast<signed int>(base_unit->name().length()), 4), ' ');
        const auto output_str = output_head.str();
        std::cout << output_str << std::flush;

        auto test = [&base_unit](bool & result, std::atomic_bool & finished)->void {
            result = base_unit->run();
            finished = true;
        };

        bool result;
        std::atomic_bool finished = false;
        std::thread worker(test, std::ref(result), std::ref(finished));
        int counter = 0;
        int seconds = 0;
        for (;;)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            counter++;
            if (counter == 100) {
                std::cout << output_str << "  " << color::color(2,3,4) << ++seconds << " s" << color::no_color() << std::flush;
                counter = 0;
            }

            if (finished)
            {
                if (worker.joinable()) {
                    worker.join();
                }

                break;
            }
        }

        if (result) {
            std::cout << output_str << color::color(0,5,0) << "  PASSED: " << color::color(0,2,2) << base_unit->success() << "\r" << std::flush;
            success++;
        } else {
            std::cout << output_str << color::color(5,0,0) << "  FAILED: " << base_unit->failure() << "\n";
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

#endif
