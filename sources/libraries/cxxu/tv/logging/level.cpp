#include <vector>

#include <cxxu/logging/level.hpp>

namespace cxxu {
namespace logging {

const char*
make_level(level l)
{
    static std::vector<level_desc> desc_ = {
        { level::none, "" },
        { level::info, "I: " },
        { level::warning, "W: " },
        { level::error, "E: " },
        { level::die, "E: " }
    };

    return desc_[(std::size_t) l].s;
}

} // namespace logging
} // namespace cxxu
