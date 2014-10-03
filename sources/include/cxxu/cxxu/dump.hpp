#ifndef __CXXU_DUMP_H__
#define __CXXU_DUMP_H__

#include <iostream>
#include <string>

#include <timevault/config.h>

namespace cxxu {

TIMEVAULT_API void hex_dump(std::ostream& os, const void* b, unsigned int count);
TIMEVAULT_API void hex_dump(const void* b, unsigned int count);

void hex_dump(const std::string& s);

template <typename T>
void hex_dump(const T& t)
{
    hex_dump(std::cout, (const void*) &t, sizeof(T));
}

} // namespace cxxu

#endif // __CXXU_DUMP_H__
