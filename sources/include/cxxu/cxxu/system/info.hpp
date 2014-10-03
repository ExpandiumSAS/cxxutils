#ifndef __CXXU_SYSTEM_INFO_H__
#define __CXXU_SYSTEM_INFO_H__

#include <stdint.h>

#include <timevault/config.h>

namespace cxxu {
namespace system {

class TIMEVAULT_API info
{
public:
    info();
    ~info();

    uint32_t logical_cpus() const;

private:
    void load();
    void cpu_info();

    uint32_t logical_cpus_;
};

} // namespace system
} // namespace cxxu

#endif // __CXXU_SYSTEM_INFO_H__
