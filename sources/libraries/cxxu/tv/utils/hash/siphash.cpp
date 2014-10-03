#include <cxxu/hash/siphash.hpp>
#include <cxxu/hash/siphash_simd.hpp>

namespace cxxu {
namespace hash {

siphash::siphash()
{
    for (uint8_t i = 0; i < 16; i++) {
        key_[i] = i;
    }
}

siphash::~siphash()
{}

uint64_t
siphash::operator()(const void* msg, std::size_t size)
{
    return siphash_simd(key_, (const uint8_t*) msg, size);
}

} // namespace hash
} // namespace cxxu
