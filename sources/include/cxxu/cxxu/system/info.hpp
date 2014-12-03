#ifndef __CXXU_SYSTEM_INFO_H__
#define __CXXU_SYSTEM_INFO_H__

#include <stdint.h>

#include <cxxutils/config.h>

namespace cxxu {
namespace system {

class CXXUTILS_API info
{
public:
    info();
    ~info();

    uint32_t logical_cpus() const;
    uint64_t physical_memory() const;

private:
    void load();
    void cpu_info();
    void mem_info();

    uint32_t logical_cpus_;
    uint64_t physical_memory_;
};

} // namespace system
} // namespace cxxu

#endif // __CXXU_SYSTEM_INFO_H__
