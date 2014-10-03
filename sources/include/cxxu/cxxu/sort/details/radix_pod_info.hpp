#ifndef __CXXU_SORT_DETAILS_RADIX_POD_INFO_H__
#define __CXXU_SORT_DETAILS_RADIX_POD_INFO_H__

#include <stdint.h>
#include <limits>
#include <cxxu/pod_info.hpp>

#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/equal_to.hpp>

namespace cxxu {
namespace sort {
namespace details {

namespace mpl = boost::mpl;

template <typename PodType>
struct radix_pod_info {
    // Real types are processed as integers
    typedef mpl::map<
        mpl::pair<float, uint32_t>,
        mpl::pair<double, uint64_t>
    > integer_map;

    typedef PodType pod_type;

    // Base type to process, either the same as pod_type or a corresponding
    // integer type for real types
    typedef typename mpl::eval_if<
        typename mpl::has_key<integer_map, pod_type>::type
      , mpl::at<integer_map, pod_type>
      , mpl::identity<pod_type>
    >::type radix_pod_type;
};

} // namespace details
} // namespace sort
} // namespace cxxu

#endif // __CXXU_SORT_DETAILS_RADIX_POD_INFO_H__
