#include "helper/configuration.h"
#include "helper/get_env.h"
#include <filesystem>
#include <fstream>
#include <regex>
#include "helper/err_type.h"

std::string clean_line(const std::string& line)
{
    return line.substr(0, line.find_first_of('#'));
}

std::string get_section(const std::string& line)
{
    const std::regex pattern(R"(\s*\[([^\]]+)\]\s*)");
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

    pair.first = pair.first.substr(0, std::min(pair.first.find_last_not_of(' ') + 1, pair.first.size())); // remove tailing spaces
    pair.second = pair.second.substr(0, std::min(pair.second.find_last_not_of(' ') + 1, pair.second.size()));
    return pair;
}

std::string replace_all(std::string & original, const std::string & target, const std::string & replacement);
std::string process_value(std::string value)
{
    std::map < std::string, std::string > replace_list;
    const std::regex pattern(R"(\%[\w]+\%)");
    const auto matches_begin = std::sregex_iterator(begin(value), end(value), pattern);
    const auto matches_end = std::sregex_iterator();
    for (std::sregex_iterator i = matches_begin; i != matches_end; ++i)
    {
        const auto match = i->str();
        auto env_key = i->str();
        replace_all(env_key, "%", "");
        replace_list[match] = get_env(env_key);
    }

    for (const auto & [key, env] : replace_list)
    {
        replace_all(value, key, env);
    }

    return value;
}

configuration::configuration(const std::string& path)
{
    std::ifstream file(path);
    if (!file) {
        throw runtime_error("Cannot open config file " + path);
    }

    std::string line;
    std::string section;
    int line_num = 0;
    while (std::getline(file, line))
    {
        line_num++;
        std::string section_tmp = get_section(clean_line(line));
        const auto [key, value] = get_pair(clean_line(line));
        if (!section_tmp.empty())
        {
            section = section_tmp;
        }
        else
        {
            if (!key.empty())
            {
                if (section.empty()) {
                    throw runtime_error("Line: " + std::to_string(line_num) + ": section head is empty");
                }
                config_[section][key].push_back(process_value(value));
            }
        }
    }
}
