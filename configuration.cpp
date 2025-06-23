#include "configuration.h"
#include <fstream>
#include <regex>

#include "include/err_type.h"

std::string clean_line(const std::string& line)
{
    return line.substr(0, line.find_first_of('#'));
}

std::string get_section(const std::string& line)
{
    const std::regex pattern(R"(\[([^\]]+)\])");
    if (std::smatch matches; std::regex_match(line, matches, pattern) && matches.size() > 1)
    {
        return matches[1];
    }

    return "";
}

std::pair <std::string, std::string> get_pair(const std::string& line)
{
    std::pair <std::string, std::string> pair;
    const std::regex pattern(R"(\s*([^=]+)\s*=\s*(.*)\s*)");
    if (std::smatch matches; std::regex_match(line, matches, pattern) && matches.size() > 2)
    {
        pair.first = matches[1];
        pair.second = matches[2];
    }

    return pair;
}

configuration::configuration(const std::string& path)
{
    std::ifstream file(path);
    std::string line;
    std::vector < std::pair <std::string, std::string> > pairs;
    std::string section;
    bool in_section = false;
    int line_num = 0;
    while (std::getline(file, line))
    {
        line_num++;
        section = get_section(clean_line(line));
        auto pair = get_pair(clean_line(line));
        in_section = !(section.empty());

        if (in_section)
        {
            if (!section.empty() && !pairs.empty())
            {
                config_.emplace(section, pairs);
                section.clear();
                pairs.clear();
            }
            else if (section.empty() && !pairs.empty()) // section head is empty but I have key pairs
            {
                throw runtime_error("Line: " + std::to_string(line_num) + ": section head is empty");
            }
        }
        else
        {
            if (!pair.first.empty())
            {
                pairs.emplace_back(pair);
            }
        }
    }

    if (!section.empty() && !pairs.empty())
    {
        config_.emplace(section, pairs);
    }
    else if (section.empty() && !pairs.empty())
    {
        throw runtime_error("Line: " + std::to_string(line_num) + ": section head is empty");
    }
}
