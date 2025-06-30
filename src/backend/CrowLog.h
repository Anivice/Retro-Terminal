#ifndef CROWLOG_H
#define CROWLOG_H

#include "crow.h"

extern
class CrowLogHandler_ final : public crow::ILogHandler
{
    void debug(const std::string & message);
    void info(const std::string & message);
    void warning(const std::string & message);
    void error(const std::string & message);

public:
    void log(std::string message, crow::LogLevel level) override;
} CrowLogHandler;

#endif //CROWLOG_H
