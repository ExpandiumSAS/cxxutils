#ifndef __CXXU_HASH_SIPHASH_H__
#define __CXXU_HASH_SIPHASH_H__

#include <stdint.h>
#include <cstddef>
#include <cstdint>

#include <cxxutils/config.h>

namespace cxxu {
namespace hash {

class CXXUTILS_API siphash
{
public:
    siphash();
    virtual ~siphash();

    uint64_t operator()(const void* msg, std::size_t size);

private:
    uint8_t key_[16];
};

} // namespace hash
} // namespace cxxu

#endif // __CXXU_HASH_SIPHASH_H__
