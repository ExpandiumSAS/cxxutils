#ifndef __CXXU_SORT_DETAILS_RADIXER_TRAITS_H__
#define __CXXU_SORT_DETAILS_RADIXER_TRAITS_H__

#include <stdint.h>
#include <type_traits>

#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/logical.hpp>

#include <cxxu/pod_info.hpp>

namespace cxxu {
namespace sort {
namespace details {

namespace mpl = boost::mpl;

template <
    int Index,
    int LastIndex,
    bool Signed
>
struct is_last_index_and_signed {
    typedef typename mpl::and_<
        mpl::equal_to<
            mpl::int_<Index>, mpl::int_<LastIndex>
        >,
        mpl::bool_<Signed>
    >::type type;
};

template <typename PodType>
struct radixer_traits {
    typedef PodType pod_type;

    typedef mpl::map<
        mpl::pair<mpl::int_<8>, mpl::int_<0> >,
        mpl::pair<mpl::int_<16>, mpl::int_<8> >,
        mpl::pair<mpl::int_<32>, mpl::int_<11> >,
        mpl::pair<mpl::int_<64>, mpl::int_<11> >
    > radix_bits_map;

    typedef mpl::map<
        mpl::pair<mpl::int_<8>, mpl::int_<0x80> >,
        mpl::pair<mpl::int_<16>, mpl::int_<0x80> >,
        mpl::pair<mpl::int_<32>, mpl::int_<0x200> >,
        mpl::pair<mpl::int_<64>, mpl::int_<0x400> >
    > radix_xor_mask_map;

    typedef mpl::map<
        mpl::pair<mpl::int_<8>, uint32_t>,
        mpl::pair<mpl::int_<16>, uint32_t>,
        mpl::pair<mpl::int_<32>, uint32_t>,
        mpl::pair<mpl::int_<64>, pod_type>
    > radix_type_map;

    typedef mpl::map<
        mpl::pair<mpl::int_<8>, int32_t>,
        mpl::pair<mpl::int_<16>, int32_t>,
        mpl::pair<mpl::int_<32>, int32_t>,
        mpl::pair<mpl::int_<64>, int64_t>
    > radix_hist_type_map;

    typedef mpl::map<
        mpl::pair<mpl::int_<8>, uint32_t>,
        mpl::pair<mpl::int_<16>, uint32_t>,
        mpl::pair<mpl::int_<32>, pod_type>,
        mpl::pair<mpl::int_<64>, uint64_t>
    > radix_pos_type_map;

    typedef cxxu::pod_info<pod_type> pod_info;

    typedef typename mpl::at<
        radix_type_map, mpl::int_<pod_info::bits>
    >::type radix_type;

    typedef typename mpl::at<
        radix_hist_type_map, mpl::int_<pod_info::bits>
    >::type radix_hist_type;

    typedef typename mpl::at<
        radix_pos_type_map, mpl::int_<pod_info::bits>
    >::type radix_pos_type;

    typedef typename std::make_unsigned<pod_type>::type upod_type;

    // Radix bits to use base on pod type
    static const radix_type radix_bits =
        mpl::at<radix_bits_map, mpl::int_<pod_info::bits> >::type::value;

    static const radix_type mask =
        radix_bits > 0
        ? (1 << radix_bits) - 1
        : std::numeric_limits<upod_type>::max()
        ;
    static const radix_type xor_mask =
        mpl::at<radix_xor_mask_map, mpl::int_<pod_info::bits> >::type::value;

    static const bool is_signed = pod_info::is_signed;
    static const int last_index =
        radix_bits > 0
        ? (pod_info::bits - 1) / radix_bits
        : 0
        ;
    static const int hist_size =
        radix_bits > 0
        ? (1 << radix_bits)
        : std::numeric_limits<upod_type>::max() + 1
        ;
    static const int hist_count = last_index + 1;

    typedef typename is_last_index_and_signed<
        last_index, last_index, true
    >::type is_last_index_and_signed_type;
};

} // namespace details
} // namespace sort
} // namespace cxxu

#endif // __CXXU_SORT_DETAILS_RADIXER_TRAITS_H__
