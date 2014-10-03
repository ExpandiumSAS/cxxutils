#include <iostream>
#include <cstdlib>

#include <cxxu/logging/clog_writer.hpp>
#include <cxxu/utils.hpp>

namespace cxxu {
namespace logging {

clog_writer::clog_writer()
: interactive_(cxxu::interactive())
{}

clog_writer::~clog_writer()
{}

void
clog_writer::operator()(level l, const std::string& what)
{
    if (!interactive_ && (l != level::error || l != level::die)) {
        return;
    }

    // If running under prove or TAP::Harness, don't ruin the TAP output
    bool harness_active = (std::getenv("HARNESS_ACTIVE") != nullptr);

    if (what.size() > 0) {
        std::unique_lock<std::mutex> lock(m_);

        std::clog
            << (harness_active ? "# " : "")
            << (interactive_ ? "" : make_timestamp())
            << make_level(l)
            << what
            << std::endl;
    }
}

} // namespace logging
} // namespace cxxu
