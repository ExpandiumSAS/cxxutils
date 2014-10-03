#ifndef __CXXU_POD_INFO_H__
#define __CXXU_POD_INFO_H__

#include <stdint.h>

#include <limits>
#include <type_traits>

#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>

namespace cxxu {

namespace mpl = boost::mpl;

template <bool IsSigned>
struct signed_info
{
    static const bool value = true;
    typedef std::true_type type;
};

template <>
struct signed_info<false>
{
    static const bool value = false;
    typedef std::false_type type;
};

typedef signed_info<true>::type type_is_signed;
typedef signed_info<false>::type type_is_not_signed;

template <typename PodType>
struct pod_info {
    typedef PodType pod_type;
    typedef typename mpl::eval_if<
        std::is_integral<pod_type>,
        std::make_unsigned<pod_type>,
        mpl::identity<pod_type>
    >::type upod_type;

    static const uint8_t char_bits = 8;
    static const uint8_t bits = char_bits * sizeof(pod_type);
    static const uint8_t size = sizeof(pod_type);

    static const bool is_signed = std::numeric_limits<pod_type>::is_signed;
    typedef typename signed_info<is_signed>::type is_signed_type;

    typedef typename mpl::equal_to<mpl::int_<bits>, mpl::int_<8>>::type is_8bits;
    typedef typename mpl::equal_to<mpl::int_<bits>, mpl::int_<16>>::type is_16bits;
    typedef typename mpl::equal_to<mpl::int_<bits>, mpl::int_<32>>::type is_32bits;
    typedef typename mpl::equal_to<mpl::int_<bits>, mpl::int_<64>>::type is_64bits;
};

} // namespace cxxu

#endif // __CXXU_POD_INFO_H__
