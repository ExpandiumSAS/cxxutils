#ifndef __CXXU_SORT_DETAILS_RADIX_CONVERTERS_H__
#define __CXXU_SORT_DETAILS_RADIX_CONVERTERS_H__

#include <cxxu/sort/details/radix_pod_info.hpp>

namespace cxxu {
namespace sort {
namespace details {

inline
uint32_t radix_float_flip(uint32_t f)
{
    uint32_t mask = -(int32_t)(f >> 31) | 0x80000000;
    return f ^ mask;
}

inline
uint32_t radix_inv_float_flip(uint32_t f)
{
    uint32_t mask = ((f >> 31) - 1) | 0x80000000;
    return f ^ mask;
}

inline
uint64_t radix_double_flip(uint64_t d)
{
    uint64_t mask = -(int64_t)(d >> 63) | 0x8000000000000000ULL;
    return d ^ mask;
}

inline
uint64_t radix_inv_double_flip(uint64_t d)
{
    uint64_t mask = ((d >> 63) - 1) | 0x8000000000000000ULL;
    return d ^ mask;
}

template <typename PodType>
struct radix_converter {
    typedef PodType pod_type;

    static void pre(pod_type* array, size_t size)
    {}

    static void post(pod_type* array, size_t size)
    {}
};

template <>
struct radix_converter<float> {
    typedef radix_pod_info<float> info;

    static void pre(info::radix_pod_type* array, size_t size) {
        for (unsigned int i = 0; i < size; i++) {
            array[i] = radix_float_flip(array[i]);
        }
    }

    static void post(info::radix_pod_type* array, size_t size) {
        for (unsigned int i = 0; i < size; i++) {
            array[i] = radix_inv_float_flip(array[i]);
        }
    }
};

template <>
struct radix_converter<double> {
    typedef radix_pod_info<double> info;

    static void pre(info::radix_pod_type* array, size_t size) {
        for (unsigned int i = 0; i < size; i++) {
            array[i] = radix_double_flip(array[i]);
        }
    }

    static void post(info::radix_pod_type* array, size_t size) {
        for (unsigned int i = 0; i < size; i++) {
            array[i] = radix_inv_double_flip(array[i]);
        }
    }
};

} // namespace details
} // namespace sort
} // namespace cxxu

#endif // __CXXU_SORT_DETAILS_RADIX_CONVERTERS_H__
