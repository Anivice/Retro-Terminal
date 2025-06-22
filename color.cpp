#include <string>
#include "get_env.h"
#include "color.h"

std::atomic < const char * > no_color = "\033[0m";

std::string color(const int r, const int g, const int b, const int br, const int bg, const int bb)
{
    if (const auto color_env = get_env(COLOR);
        color_env == "never" || color_env == "none" ||
        color_env == "off" || color_env == "no" || color_env == "n")
    {
        return "";
    }

    return color(r, g, b) + bg_color(br, bg, bb);
}

std::string color(int r, int g, int b)
{
    if (const auto color_env = get_env(COLOR);
        color_env == "never" || color_env == "none" ||
        color_env == "off" || color_env == "no" || color_env == "n")
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
    if (const auto color_env = get_env(COLOR);
    color_env == "never" || color_env == "none" ||
    color_env == "off" || color_env == "no" || color_env == "n")
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
