#include <thread>
#include "test/test.h"

class simple_unit_test_ final : test::unit_t {
public:
    std::string success() override {
        return "NORMAL UNIT TEST";
    }

    std::string failure() override {
        return "NORMAL UNIT TEST FAILED";
    }

    bool run() override {
        return true;
    }
} simple_unit_test;

class delay_unit_test_ final : test::unit_t {
public:
    std::string success() override {
        return "NORMAL UNIT TEST (delayed)";
    }

    std::string failure() override {
        return "NORMAL UNIT TEST FAILED (delayed)";
    }

    bool run() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return true;
    }
} delay_unit_test;

class failed_unit_test_ final : test::unit_t {
public:
    std::string success() override {
        exit(EXIT_FAILURE);
    }

    std::string failure() override {
        return "DESIGNED FAILURE";
    }

    bool run() override {
        return false;
    }
} failed_unit_test;

class failed_delay_test_ final : test::unit_t {
public:
    std::string success() override {
        exit(EXIT_FAILURE);
    }

    std::string failure() override {
        return "DESIGNED FAILURE (delayed)";
    }

    bool run() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return false;
    }
} failed_delay_test;

std::vector < void * > test::unit_tests = {
    &failed_delay_test, &failed_unit_test, &simple_unit_test, &delay_unit_test,
};
