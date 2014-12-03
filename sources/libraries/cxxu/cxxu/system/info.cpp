#ifdef DARWIN
#include <sys/sysctl.h>
#endif // DARWIN

#ifdef LINUX
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#endif // LINUX

#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#if defined(BSD)
#include <sys/sysctl.h>
#endif


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

uint64_t
info::physical_memory() const
{ return physical_memory_; }

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

void 
info::mem_info()
{
    physical_memory_ = 0;
    bool ok = false;

#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    /* UNIX variants. ------------------------------------------- */
    /* Prefer sysctl() over sysconf() except sysctl() HW_REALMEM and HW_PHYSMEM */

#if defined(CTL_HW) && (defined(HW_MEMSIZE) || defined(HW_PHYSMEM64))
    int mib[2];
    mib[0] = CTL_HW;
        #if defined(HW_MEMSIZE)
    mib[1] = HW_MEMSIZE;            /* OSX. --------------------- */
        #elif defined(HW_PHYSMEM64)
    mib[1] = HW_PHYSMEM64;          /* NetBSD, OpenBSD. --------- */
        #endif
    int64_t size = 0;               /* 64-bit */
    size_t len = sizeof( size );
    if ( sysctl( mib, 2, &size, &len, NULL, 0 ) == 0 ) {
        ok = true;
        physical_memory_ (uint64_t)size;
    } 

#elif defined(_SC_AIX_REALMEM)
    /* AIX. ----------------------------------------------------- */
    physical_memory_ = (uint64_t)sysconf( _SC_AIX_REALMEM ) * (uint64_t)1024L;

#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
    /* FreeBSD, Linux, OpenBSD, and Solaris. -------------------- */
    physical_memory_ = (uint64_t)sysconf( _SC_PHYS_PAGES ) * (uint64_t)sysconf( _SC_PAGESIZE );

#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGE_SIZE)
    /* Legacy. -------------------------------------------------- */
    physical_memory_ = (uint64_t)sysconf( _SC_PHYS_PAGES ) * (size_t)sysconf( _SC_PAGE_SIZE );

#elif defined(CTL_HW) && (defined(HW_PHYSMEM) || defined(HW_REALMEM))
    /* DragonFly BSD, FreeBSD, NetBSD, OpenBSD, and OSX. -------- */
    int mib[2];
    mib[0] = CTL_HW;
#if defined(HW_REALMEM)
    mib[1] = HW_REALMEM;        /* FreeBSD. ----------------- */
#elif defined(HW_PYSMEM)
    mib[1] = HW_PHYSMEM;        /* Others. ------------------ */
#endif
    unsigned int size = 0;      /* 32-bit */
    size_t len = sizeof( size );
    if ( sysctl( mib, 2, &size, &len, NULL, 0 ) == 0 ) {
        physical_memory_ = (uint64_t)size;
    }
    
#endif /* sysctl and sysconf variants */
#endif
    ok &= (physical_memory_ > 0);
    if (!ok) {
        CXXU_DIE("failed to physical memory size");
    }
}

} // namespace system
} // namespace cxxu
