#include <ctime>
#include <cstdlib>
#include <chrono>

#include <cxxu/logging/log_writer.hpp>

namespace cxxu {
namespace logging {

std::string
log_writer::make_timestamp()
{
    // What a hassle just to format a time string...
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto secs = std::chrono::duration_cast<
        std::chrono::seconds
    >(duration).count();
    auto millis = std::chrono::duration_cast<
        std::chrono::milliseconds
    >(duration).count();
    int ms = millis - (secs * 1000);

    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char tstr[20];
    char tsstr[20];
    std::strftime(tstr, 20, "%m.%d %H:%M:%S", std::localtime(&t));
    std::snprintf(tsstr, 20, "%s.%03d ", tstr, ms);

    return std::string(tsstr);
}

} // namespace logging
} // namespace cxxu
