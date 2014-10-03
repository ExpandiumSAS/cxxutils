#ifndef __CXXU_SORT_DETAILS_RADIXER_H__
#define __CXXU_SORT_DETAILS_RADIXER_H__

#include <stdint.h>
#include <iostream>
#include <type_traits>

#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/logical.hpp>

#include <cxxu/pod_info.hpp>
#include <cxxu/sort/details/histogram.hpp>
#include <cxxu/sort/details/radixer_traits.hpp>

namespace cxxu {
namespace sort {
namespace details {

namespace mpl = boost::mpl;

template <typename PodType, std::size_t FixedSize = 0>
struct radixer {
    typedef PodType pod_type;
    typedef radixer<PodType, FixedSize> this_type;
    typedef radixer_traits<PodType> traits;

    typedef histogram_maker<traits::last_index, this_type> hist_maker;

    typedef typename traits::radix_type radix_type;
    typedef typename traits::radix_hist_type hist_type;
    typedef typename traits::radix_pos_type pos_type;
    typedef typename traits::is_last_index_and_signed_type is_last_index_and_signed_type;
    static const auto hist_size = traits::hist_size;
    static const auto hist_count = traits::hist_count;

    // Sort source data
    void sort(pod_type* source, size_t size) {
        pod_type* dest;

        if (FixedSize == 0) {
            dest = new pod_type[size];
        } else {
            dest = fixed_dest_;
        }

        sort(dest, source, size);

        if (FixedSize == 0) {
            delete [] dest;
        }
    }

    void sort(pod_type* dest, pod_type* source, size_t size) {
        hist_type hist[hist_size * hist_count];

        std::memset(hist, 0, sizeof(hist));

        // Make histograms of input values
        make_hist(hist, source, size);
        process(dest, source, size, hist);

        if (hist_count % 2 == 1) {
            std::memcpy(source, dest, size * sizeof(pod_type));
        }
    }

    // Sort source data and store sorted indexes
    void make_sort_indices(std::size_t* indices, pod_type* source, size_t size) {
        std::size_t* dest = new std::size_t[size];

        make_sort_indices(dest, indices, source, size);

        delete [] dest;
    }

    void make_sort_indices(
        std::size_t* idest, std::size_t* isource,
        pod_type* source, size_t size
    ) {
        hist_type hist[hist_size * hist_count];

        std::memset(hist, 0, sizeof(hist));

        // Make histograms of input values
        make_hist(hist, source, size);
        process_indices(idest, isource, size, source, hist);

        if (hist_count % 2 == 1) {
            std::memcpy(isource, idest, size * sizeof(std::size_t));
        }
    }

    // Sort source data and store sorted indexes and sort orders
    void make_sort_indices_and_orders(
        std::size_t* indices,
        std::size_t* orders,
        pod_type* source, size_t size
    ) {
        std::size_t* idest = new std::size_t[size];
        std::size_t* odest = new std::size_t[size];

        make_sort_indices_and_orders(idest, indices, odest, orders, source, size);

        delete [] odest;
        delete [] idest;
    }

    void make_sort_indices_and_orders(
        std::size_t* idest, std::size_t* isource,
        std::size_t* odest, std::size_t* osource,
        pod_type* source, size_t size
    ) {
        hist_type hist[hist_size * hist_count];

        std::memset(hist, 0, sizeof(hist));

        // Make histograms of input values
        make_hist(hist, source, size);
        process_indices(idest, isource, odest, osource, size, source, hist);

        if (hist_count % 2 == 1) {
            std::memcpy(isource, idest, size * sizeof(std::size_t));
            std::memcpy(osource, odest, size * sizeof(std::size_t));
        }
    }

private:
    template <int, typename> friend class histogram;
    template <int, typename> friend class histogram_maker;

    // Common radix extractor for all types, provided required Index
    // is valid and pod type is not signed
    template <int Index>
    inline
    radix_type
    get(pod_type v) {
        return get<Index>(
            v,
            typename is_last_index_and_signed<
                Index,
                traits::last_index,
                traits::is_signed
            >::type()
        );
    }

    // Last radix extractor for signed types
    template <int Index>
    inline
    radix_type
    get(pod_type v, is_last_index_and_signed_type t) {
        radix_type r =
            (
                (((radix_type) v) >> (traits::radix_bits * Index))
                &
                traits::mask
            )
            ^
            traits::xor_mask
            ;

        // std::cout
        //     << "index:" << Index
        //     << ", radix for " << v << " is " << r << "\n"
        //     << "sv:" << (radix_bits * Index)
        //     << ", xv:" << xor_mask
        //     << std::endl
        //     ;

        return r;
    }

    // Normal radix extractor
    template <int Index, typename NormalRadix>
    static inline
    radix_type
    get(pod_type v, NormalRadix t) {
        return ((((radix_type) v) >> (traits::radix_bits * Index)) & traits::mask);
    }

    void make_hist(hist_type* hist, pod_type* source, std::size_t size)
    {
        for (std::size_t i = 0; i < size; i++) {
            hist_maker::make(*this, hist, source, i);
        }

        hist_type sums[hist_size];

        std::memset((void*) sums, 0, sizeof(sums));

        for (std::size_t i = 0; i < hist_size; i++) {
            hist_maker::sum(sums, hist, i);
        }
    }

    template <typename... Args>
    void process(Args&&... args)
    {
        hist_maker::process(*this, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void process_indices(Args&&... args)
    {
        hist_maker::process_indices(*this, std::forward<Args>(args)...);
    }

    pod_type fixed_dest_[FixedSize];
};

} // namespace details
} // namespace sort
} // namespace cxxu

#endif // __CXXU_SORT_DETAILS_RADIXER_H__
