#ifdef DARWIN
#include <sys/sysctl.h>
#endif // DARWIN

#ifdef LINUX
#include <unistd.h>
#endif // LINUX

#include <cxxu/system/info.hpp>
#include <cxxu/logging.hpp>

namespace cxxu {
namespace system {

info::info()
{
    load();
}

info::~info()
{}

uint32_t
info::logical_cpus() const
{ return logical_cpus_; }

void
info::load()
{
    cpu_info();
}

void
info::cpu_info()
{
    logical_cpus_ = 1;
    bool ok = true;

#ifdef DARWIN
    std::size_t length = sizeof(logical_cpus_);

    if (sysctlbyname("hw.logicalcpu_max", &logical_cpus_, &length, NULL, 0) != 0) {
        ok = false;
    }
#endif // DARWIN

#ifdef LINUX
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);

    if (nprocs == -1) {
        ok = false;
    }

    logical_cpus_ = static_cast<uint32_t>(nprocs);
#endif // LINUX

    if (!ok) {
        CXXU_DIE("failed to get logical cpu count");
    }
}

} // namespace system
} // namespace cxxu
