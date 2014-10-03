#ifndef __CXXU_PACK_BIT_PACKING_TABLES_H__
#define __CXXU_PACK_BIT_PACKING_TABLES_H__

#include <limits>
#include <stdexcept>
#include <sstream>

#include <boost/mpl/if.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/int.hpp>
#include <boost/utility/enable_if.hpp>

#include <cxxu/pack/bit_packing_8.hpp>
#include <cxxu/pack/bit_packing_16.hpp>
#include <cxxu/pack/bit_packing_32.hpp>
#include <cxxu/pack/bit_packing_64.hpp>

#include <cxxu/pod_info.hpp>

namespace cxxu {
namespace pack {

namespace mpl = boost::mpl;

// Pack/unpack function signature generators
template <typename PodType>
struct pack_function {
    typedef PodType pod_type;

    typedef void (*function_type)(
        pod_type* out_ptr,
        const pod_type* in_ptr,
        uint32_t n
    );
};

template <typename PodType>
struct unpack_function {
    typedef PodType pod_type;

    typedef void (*function_type)(
        pod_type* out_ptr,
        const pod_type* in_ptr,
        uint32_t n
    );
};

inline
void*
get_packer(uint32_t pod_bits, uint32_t bits, bool simd = false)
{
    void* func = 0;

    switch (pod_bits) {
        case 8:
            if (simd) {
                func = (void*) packer_8_simd_table[bits];
            } else {
                func = (void*) packer_8_table[bits];
            }
            break;
        case 16:
            if (simd) {
                func = (void*) packer_16_simd_table[bits];
            } else {
                func = (void*) packer_16_table[bits];
            }
            break;
        case 32:
            if (simd) {
                func = (void*) packer_32_simd_table[bits];
            } else {
                func = (void*) packer_32_table[bits];
            }
            break;
        case 64:
            if (simd) {
                func = (void*) packer_64_simd_table[bits];
            } else {
                func = (void*) packer_64_table[bits];
            }
            break;
        default:
            func = 0;
            break;
    }

    if (func == 0) {
        std::ostringstream oss;

        oss
            << "no packer for " << pod_bits << "@" << bits
            << " (simd: " << (simd ? "yes" : "no") << ")"
            ;

        throw std::logic_error(oss.str());
    }

    return func;
}

inline
void*
get_unpacker(uint32_t pod_bits, uint32_t bits, bool simd = false)
{
    void* func = 0;

    switch (pod_bits) {
        case 8:
            if (simd) {
                func = (void*) unpacker_8_simd_table[bits];
            } else {
                func = (void*) unpacker_8_table[bits];
            }
            break;
        case 16:
            if (simd) {
                func = (void*) unpacker_16_simd_table[bits];
            } else {
                func = (void*) unpacker_16_table[bits];
            }
            break;
        case 32:
            if (simd) {
                func = (void*) unpacker_32_simd_table[bits];
            } else {
                func = (void*) unpacker_32_table[bits];
            }
            break;
        case 64:
            if (simd) {
                func = (void*) unpacker_64_simd_table[bits];
            } else {
                func = (void*) unpacker_64_table[bits];
            }
            break;
        default:
            func = 0;
            break;
    }

    if (func == 0) {
        std::ostringstream oss;

        oss
            << "no unpacker for " << pod_bits << "@" << bits
            << " (simd: " << (simd ? "yes" : "no") << ")"
            ;

        throw std::logic_error(oss.str());
    }

    return func;
}

// pack function table getter generators
template <typename PackFunction>
struct pack_table {
    static PackFunction get(uint32_t pod_bits, uint32_t bits, bool simd = false)
    { return (PackFunction) get_packer(pod_bits, bits, simd); }
};

// pack function table getter generators
template <typename UnpackFunction>
struct unpack_table {
    static UnpackFunction get(uint32_t pod_bits, uint32_t bits, bool simd = false)
    { return (UnpackFunction) get_unpacker(pod_bits, bits, simd); }
};

template <typename PodType>
struct pack_unpack_table {
    typedef PodType pod_type;

    typedef cxxu::pod_info<pod_type> pod_info;
    static const uint32_t pod_bits = pod_info::bits;

    typedef typename mpl::if_<
        typename pod_info::is_8bits,
        uint8_t,
        typename mpl::if_<
            typename pod_info::is_16bits,
            uint16_t,
            typename mpl::if_<
                typename pod_info::is_32bits,
                uint32_t,
                uint64_t
            >::type
        >::type
    >::type pack_unpack_pod_type;

    typedef
        typename pack_function<pack_unpack_pod_type>::function_type
        pack_function_type;

    typedef
        typename unpack_function<pack_unpack_pod_type>::function_type
        unpack_function_type;

    static
    pack_function_type
    pack_function(uint32_t bits, bool simd = false)
    { return pack_table<pack_function_type>::get(pod_bits, bits, simd); }

    static
    unpack_function_type
    unpack_function(uint32_t bits, bool simd = false)
    { return unpack_table<unpack_function_type>::get(pod_bits, bits, simd); }
};

} // namespace pack
} // namespace cxxu

#endif // __CXXU_PACK_BIT_PACKING_TABLES_H__
