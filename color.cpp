#include <string>
#include "get_env.h"
#include "color.h"
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include "cpp_assert.h"
#include <atomic>

std::atomic_bool g_no_color = false;

bool is_no_color()
{
    auto color_env = get_env(COLOR);
    std::ranges::transform(color_env, color_env.begin(), ::tolower);

    if (color_env == "always")
    {
        return false;
    }

    const bool no_color_from_env = color_env == "never" || color_env == "none" || color_env == "off"
                            || color_env == "no" || color_env == "n" || color_env == "0" || color_env == "false";
    bool is_terminal = false;
    struct stat st{};
    assert_short(fstat(STDOUT_FILENO, &st) != -1);
    if (isatty(STDOUT_FILENO))
    {
        is_terminal = true;
    }

    return no_color_from_env || !is_terminal || g_no_color;
}

std::string no_color()
{
    if (!is_no_color())
    {
        return "\033[0m";
    }

    return "";
}

std::string color(const int r, const int g, const int b, const int br, const int bg, const int bb)
{
    if (is_no_color())
    {
        return "";
    }

    return color(r, g, b) + bg_color(br, bg, bb);
}

std::string color(int r, int g, int b)
{
    if (is_no_color())
    {
        return "";
    }

    auto constrain = [](int var, int min, int max)->int
    {
        var = std::max(var, min);
        var = std::min(var, max);
        return var;
    };

    r = constrain(r, 0, 5);
    g = constrain(g, 0, 5);
    b = constrain(b, 0, 5);

    const int scale = 16 + 36 * r + 6 * g + b;
    return "\033[38;5;" + std::to_string(scale) + "m";
}

std::string bg_color(int r, int g, int b)
{
    if (is_no_color())
    {
        return "";
    }

    auto constrain = [](int var, int min, int max)->int
    {
        var = std::max(var, min);
        var = std::min(var, max);
        return var;
    };

    r = constrain(r, 0, 5);
    g = constrain(g, 0, 5);
    b = constrain(b, 0, 5);

    const int scale = 16 + 36 * r + 6 * g + b;
    return "\033[48;5;" + std::to_string(scale) + "m";
}
