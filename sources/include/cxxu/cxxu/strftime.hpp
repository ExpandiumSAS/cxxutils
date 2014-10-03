#ifndef __CXXU_STRFTIME_H__
#define __CXXU_STRFTIME_H__

#include <time.h>

#include <timevault/config.h>

namespace cxxu {

TIMEVAULT_API
size_t
strftime(char *const s, const size_t maxsize,
    const char *const format, const struct tm *const t, int msec);

} // namespace cxxu

#endif // __CXXU_STRFTIME_H__
