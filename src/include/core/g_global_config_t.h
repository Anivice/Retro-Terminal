#ifndef G_GLOBAL_CONFIG_T_H
#define G_GLOBAL_CONFIG_T_H

#include <memory>
#include <mutex>
#include "helper/cpp_assert.h"
#include "helper/err_type.h"
#include "core/configuration.h"

struct list_view_t{};

extern
class g_global_config_t {
    std::unique_ptr < configuration > config;
    std::mutex mutex;
public:
    void initialize(const std::string & config_file);

    template < typename Type >
    requires (std::is_same_v<Type, std::string>
        || std::is_integral_v<Type>
        || std::is_floating_point_v<Type>
        || std::is_same_v<Type, list_view_t>)
    Type get(const std::string & key)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!config) {
            throw runtime_error("Configuration does not exist");
        }

        // 1. split section
        const auto section_end = key.find_first_of('.');
        if (section_end == std::string::npos) {
            throw runtime_error("Malformed query");
        }

        const std::string section = key.substr(0, section_end);
        const std::string section_key = key.substr(section_end + 1);
        std::vector<std::string> section_key_value;
        try {
            section_key_value = config->config.at(section).at(section_key);
        }
        catch (const std::out_of_range&)
        {
            if constexpr (std::is_integral_v<Type> || std::is_floating_point_v<Type>) {
                return -1;
            }

            if constexpr (std::is_same_v<Type, std::string>) {
                return "";
            }

            throw runtime_error("No such section or key");
        }

        if constexpr (std::is_same_v<Type, list_view_t>)
        {
            return section_key_value;
        }
        else
        {
            assert_throw(section_key_value.size() == 1, "More than one key specified, but got singleton query");

            if constexpr (std::is_same_v<Type, std::string>) {
                return section_key_value.at(0);
            }

            if constexpr (std::is_integral_v<Type>) {
                return std::stoll(section_key_value.at(0));
            }

            if constexpr (std::is_floating_point_v<Type>) {
                return std::stod(section_key_value.at(0));
            }
        }

        throw runtime_error("Malformed query");
    }

} g_global_config;

#endif //G_GLOBAL_CONFIG_T_H
