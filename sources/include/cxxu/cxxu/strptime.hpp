#ifndef __CXXU_STRPTIME_H__
#define __CXXU_STRPTIME_H__

#include <time.h>

#include <cxxutils/config.h>

namespace cxxu {

CXXUTILS_API
char*
strptime(const char *buf, const char *fmt, struct tm *tm, int* msec);

} // namespace cxxu

#endif // __CXXU_STRPTIME_H__
