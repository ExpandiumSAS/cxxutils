#ifndef __CXXU_HASH_SIPHASH_SIMD_H__
#define __CXXU_HASH_SIPHASH_SIMD_H__

#include <stdint.h>

#include <cxxutils/config.h>

namespace cxxu {
namespace hash {

CXXUTILS_API
uint64_t
siphash_simd(const uint8_t key[16], const uint8_t *m, uint64_t n);

} // namespace hash
} // namespace cxxu

#endif // __CXXU_HASH_SIPHASH_SIMD_H__
