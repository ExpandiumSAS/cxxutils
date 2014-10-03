#ifndef __CXXU_SORT_DETAILS_RADIX_SORT_H__
#define __CXXU_SORT_DETAILS_RADIX_SORT_H__

#include <limits>

#include <boost/mpl/if.hpp>

#include <cxxu/pod_info.hpp>
#include <cxxu/sort/details/radix_pod_info.hpp>
#include <cxxu/sort/details/radixer.hpp>
#include <cxxu/sort/details/radix_converters.hpp>

namespace cxxu {
namespace sort {
namespace details {

namespace mpl = boost::mpl;

template <typename PodType, std::size_t FixedSize = 0>
struct radix_sort {
    typedef PodType pod_type;

    // Radix type specific information
    typedef radix_pod_info<pod_type> radix_info;

    // Pod type to use to compute radix on PodType
    typedef typename radix_info::radix_pod_type radix_pod_type;

    // Base pod info for selected radix pod type
    typedef tv::utils::pod_info<radix_pod_type> pod_info;

    // Radix implementation to use
    typedef details::radixer<radix_pod_type, FixedSize> impl_type;

    // Radix pod converter (for float/double) to use
    typedef details::radix_converter<pod_type> conv_type;
};

} // namespace details
} // namespace sort
} // namespace cxxu

#endif // __CXXU_SORT_DETAILS_RADIX_SORT_H__
