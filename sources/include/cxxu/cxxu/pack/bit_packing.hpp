#ifndef __CXXU_PACK_BIT_PACKING_H__
#define __CXXU_PACK_BIT_PACKING_H__

#include <stdint.h>

#include <stdexcept>
#include <sstream>
#include <cstdlib>

#include <cxxu/compiler.hpp>
#include <cxxu/pack/bit_packing_tables.hpp>

namespace cxxu {
namespace pack {

template <typename PodType>
struct bit_packer {
    typedef PodType pod_type;

    typedef pack_unpack_table<pod_type> table_type;
    typedef typename table_type::pack_function_type pack_function_type;
    typedef typename table_type::unpack_function_type unpack_function_type;
    typedef typename table_type::pack_unpack_pod_type pack_unpack_pod_type;

    static const uint32_t pod_bits = table_type::pod_bits;
    static const uint32_t simd_bits = 128;
    static const uint32_t simd_items = 128;
    static const uint32_t simd_items_at_a_time = simd_bits / pod_bits;

    static
    uint32_t
    compressed_size(uint32_t bits, uint32_t n)
    {
        uint32_t simd_processed = (n / simd_items_at_a_time) * simd_items_at_a_time;

        return
            compressed_size_(bits, simd_processed)
            +
            compressed_size_(bits, n - simd_processed)
            ;
    }

    static
    uint32_t
    compressed_size_(uint32_t bits, uint32_t n)
    {
        return
            cxxu::align_to_128bits(
                cxxu::byte_size_of(bits, n)
            )
            ;
    }

    static
    void*
    simd_pack(void* out, const pod_type* in, uint32_t bits, uint32_t n)
    {
        const pack_unpack_pod_type* in_ = (const pack_unpack_pod_type*) in;
        pack_unpack_pod_type* out_ = (pack_unpack_pod_type*) out;

        pack_function_type simd_packer = table_type::pack_function(bits, true);

        uint32_t simd_count = n / simd_items_at_a_time;

        simd_packer(out_, in_, simd_count);

        uint32_t processed = simd_count * simd_items_at_a_time;
        cxxu::inc_ptr(out_, compressed_size(bits, processed));

        return (void*) out_;
    }

    static
    void*
    std_pack(void* out, const pod_type* in, uint32_t bits, uint32_t n)
    {
        const pack_unpack_pod_type* in_ = (const pack_unpack_pod_type*) in;
        pack_unpack_pod_type* out_ = (pack_unpack_pod_type*) out;

        pack_function_type std_packer = table_type::pack_function(bits, false);

        std_packer(out_, in_, n);
        cxxu::inc_ptr(out_, compressed_size(bits, n));

        return (void*) out_;
    }

    static
    void*
    pack(void* out, const pod_type* in, uint32_t bits, uint32_t n)
    {
        if (unlikely(bits == 0)) {
            return out;
        }

        const pack_unpack_pod_type* in_ = (const pack_unpack_pod_type*) in;
        pack_unpack_pod_type* out_ = (pack_unpack_pod_type*) out;

        if (unlikely(bits == pod_bits)) {
            std::memcpy(out, (void*) in, n * sizeof(pod_type));
            out_ += n;
        } else {
            pack_function_type simd_packer = table_type::pack_function(bits, true);
            pack_function_type std_packer = table_type::pack_function(bits, false);

            uint32_t items_to_process = n;

            if (items_to_process >= simd_items) {
                uint32_t simd_count = items_to_process / simd_items_at_a_time;

                simd_packer(out_, in_, simd_count);

                uint32_t processed = simd_count * simd_items_at_a_time;
                items_to_process -= processed;
                in_ += processed;
                cxxu::inc_ptr(out_, compressed_size(bits, processed));
            }

            if (items_to_process > 0) {
                std_packer(out_, in_, items_to_process);
                cxxu::inc_ptr(out_, compressed_size(bits, items_to_process));
            }
        }

        return (void*) out_;
    }

    static
    const void*
    simd_unpack(pod_type* out, const void* in, uint32_t bits, uint32_t n)
    {
        const pack_unpack_pod_type* in_ = (const pack_unpack_pod_type*) in;
        pack_unpack_pod_type* out_ = (pack_unpack_pod_type*) out;

        pack_function_type simd_unpacker = table_type::unpack_function(bits, true);

        uint32_t simd_count = n / simd_items_at_a_time;

        simd_unpacker(out_, in_, simd_count);

        uint32_t processed = simd_count * simd_items_at_a_time;
        cxxu::inc_ptr(in_, compressed_size(bits, processed));

        return (void*) in_;
    }

    static
    const void*
    std_unpack(pod_type* out, const void* in, uint32_t bits, uint32_t n)
    {
        const pack_unpack_pod_type* in_ = (const pack_unpack_pod_type*) in;
        pack_unpack_pod_type* out_ = (pack_unpack_pod_type*) out;

        pack_function_type std_unpacker = table_type::unpack_function(bits, false);

        std_unpacker(out_, in_, n);
        cxxu::inc_ptr(in_, compressed_size(bits, n));

        return (void*) in_;
    }

    static
    const void*
    unpack(pod_type* out, const void* in, uint32_t bits, uint32_t n)
    {
        if (unlikely(bits == 0)) {
            return (void*) in;
        }

        const pack_unpack_pod_type* in_ = (const pack_unpack_pod_type*) in;
        pack_unpack_pod_type* out_ = (pack_unpack_pod_type*) out;

        if (unlikely(bits == pod_bits)) {
            std::memcpy((void*) out, in, n * sizeof(pod_type));
            in_ += n;
        } else {
            pack_function_type simd_unpacker = table_type::unpack_function(bits, true);
            pack_function_type std_unpacker = table_type::unpack_function(bits, false);

            uint32_t items_to_process = n;

            if (items_to_process >= simd_items) {
                uint32_t simd_count = items_to_process / simd_items_at_a_time;

                simd_unpacker(out_, in_, simd_count);

                uint32_t processed = simd_count * simd_items_at_a_time;
                items_to_process -= processed;
                cxxu::inc_ptr(in_, compressed_size(bits, processed));
                out_ += processed;
            }

            if (items_to_process > 0) {
                std_unpacker(out_, in_, items_to_process);
                cxxu::inc_ptr(in_, compressed_size(bits, items_to_process));
            }
        }

        return (void*) in_;
    }
};

} // namespace pack
} // namespace cxxu

#endif // __CXXU_PACK_BIT_PACKING_H__
