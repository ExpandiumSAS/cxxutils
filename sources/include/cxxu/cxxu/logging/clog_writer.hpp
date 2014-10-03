#ifndef __CXXU_LOGGING_CLOG_WRITER_H__
#define __CXXU_LOGGING_CLOG_WRITER_H__

#include <mutex>

#include <cxxu/logging/log_writer.hpp>

#include <timevault/config.h>

namespace cxxu {
namespace logging {

class TIMEVAULT_API clog_writer : public log_writer
{
public:
    clog_writer();
    ~clog_writer();

    void operator()(level l, const std::string& what);

private:
    bool interactive_;
    std::mutex m_;
};

} // namespace logging
} // namespace cxxu

#endif // __CXXU_LOGGING_CLOG_WRITER_H__
