#include "core/g_global_config_t.h"

g_global_config_t g_global_config;
void g_global_config_t::initialize(const std::string & config_file)
{
    config = std::make_unique<configuration>(config_file);
}
