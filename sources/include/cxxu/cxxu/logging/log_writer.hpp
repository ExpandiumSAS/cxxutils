#ifndef __CXXU_LOGGING_LOG_WRITER_H__
#define __CXXU_LOGGING_LOG_WRITER_H__

#include <string>
#include <memory>

#include <cxxutils/config.h>

#include <cxxu/logging/level.hpp>

namespace cxxu {
namespace logging {

class CXXUTILS_API log_writer
{
public:
    virtual
    void operator()(level l, const std::string& what) = 0;

    std::string make_timestamp();
};

typedef std::shared_ptr<log_writer> log_writer_ptr;

} // namespace logging
} // namespace cxxu

#endif // __CXXU_LOGGING_LOG_WRITER_H__
