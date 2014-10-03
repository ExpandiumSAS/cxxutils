#ifndef __CXXU_LOGGING_SYSLOG_WRITER_H__
#define __CXXU_LOGGING_SYSLOG_WRITER_H__

#include <string>
#include <mutex>

#include <cxxu/logging/log_writer.hpp>

#include <cxxutils/config.h>

namespace cxxu {
namespace logging {

class CXXUTILS_API syslog_writer : public log_writer
{
public:
    syslog_writer(const std::string& name = "");
    ~syslog_writer();

    void operator()(level l, const std::string& what);

private:
    std::string name_;
};

} // namespace logging
} // namespace cxxu

#endif // __CXXU_LOGGING_SYSLOG_WRITER_H__
