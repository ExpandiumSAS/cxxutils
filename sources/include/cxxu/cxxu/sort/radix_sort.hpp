#ifndef __CXXU_SORT_RADIX_H__
#define __CXXU_SORT_RADIX_H__

#include <cstring>

#include <cxxutils/config.h>
#include <cxxu/sort/details/radix_sort.hpp>

namespace cxxu {
namespace sort {

template <typename PodType, std::size_t FixedSize = 0>
struct radix_sort {
    typedef PodType pod_type;
    typedef details::radix_sort<pod_type, FixedSize> radix_type;

    typedef typename radix_type::radix_pod_type radix_pod_type;
    typedef typename radix_type::impl_type sorter_type;
    typedef typename radix_type::conv_type converter_type;

    void sort(pod_type* dest, pod_type* source, size_t size) {
        converter_type::pre((radix_pod_type*) source, size);
        sorter_type sorter;
        sorter.sort((radix_pod_type*) dest, (radix_pod_type*) source, size);
        converter_type::post((radix_pod_type*) dest, size);
    }

    void sort(pod_type* source, size_t size) {
        converter_type::pre((radix_pod_type*) source, size);
        sorter_type sorter;
        sorter.sort((radix_pod_type*) source, size);
        converter_type::post((radix_pod_type*) source, size);
    }

    void make_sort_indices(
        std::size_t* indices,
        pod_type* source,
        size_t size
    ) {
        converter_type::pre((radix_pod_type*) source, size);
        sorter_type sorter;
        sorter.make_sort_indices(indices, (radix_pod_type*) source, size);
        converter_type::post((radix_pod_type*) source, size);
    }

    void make_sort_indices_and_orders(
        std::size_t* indices,
        std::size_t* orders,
        pod_type* source,
        size_t size
    ) {
        converter_type::pre((radix_pod_type*) source, size);
        sorter_type sorter;
        sorter.make_sort_indices_and_orders(indices, orders, (radix_pod_type*) source, size);
        converter_type::post((radix_pod_type*) source, size);
    }
};

} // namespace sort
} // namespace cxxu

#endif // __CXXU_SORT_RADIX_H__
