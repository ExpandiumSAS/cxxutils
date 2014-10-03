#include <syslog.h>

#include <cxxu/logging/syslog_writer.hpp>

namespace cxxu {
namespace logging {

syslog_writer::syslog_writer(const std::string& name)
: name_(name)
{
    if (name_.empty()) {
        name_ = "timevault";
    }

    openlog(name_.c_str(), 0, LOG_DAEMON);
}

syslog_writer::~syslog_writer()
{
    closelog();
}

void
syslog_writer::operator()(level l, const std::string& what)
{
    int priority = 0;

    switch (l) {
        case level::none:
        priority = LOG_DEBUG;
        break;
        case level::info:
        priority = LOG_INFO;
        break;
        case level::warning:
        priority = LOG_WARNING;
        break;
        case level::error:
        priority = LOG_ERR;
        break;
        case level::die:
        priority = LOG_CRIT;
        break;
    }

    syslog(priority, "%s", what.c_str());
}

} // namespace logging
} // namespace cxxu
