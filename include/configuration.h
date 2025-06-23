#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <map>
#include <vector>

class configuration {
public:
    using configuration_map_t = std::map < std::string /* Section */, std::vector < std::pair < std::string /* Key */, std::string /* Value */ > > >;
    using iterator = configuration_map_t::iterator;
    using const_iterator = configuration_map_t::const_iterator;
    iterator begin() { return config_.begin(); }
    iterator end() { return config_.end(); }
    [[nodiscard]] const_iterator begin() const { return config_.begin(); }
    [[nodiscard]] const_iterator end() const { return config_.end(); }

private:
    configuration_map_t config_;

public:
    explicit configuration(const std::string & path);
};

#endif //CONFIGURATION_H
