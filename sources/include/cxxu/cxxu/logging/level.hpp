#ifndef __CXXU_LOGGING_LEVEL_H__
#define __CXXU_LOGGING_LEVEL_H__

#include <timevault/config.h>

namespace cxxu {
namespace logging {

enum class level {
    none,
    info,
    warning,
    error,
    die
};

struct level_desc
{
    level l;
    const char* s;
};

TIMEVAULT_API
const char*
make_level(level l);

} // namespace logging
} // namespace cxxu

#endif // __CXXU_LOGGING_LEVEL_H__
