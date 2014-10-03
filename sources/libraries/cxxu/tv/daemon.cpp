#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <stdlib.h>
#include <unistd.h>

#include <cxxu/daemon.hpp>
#include <cxxu/logging.hpp>
#include <cxxu/signaler.hpp>

namespace cxxu {

daemon::daemon(const std::string& name)
: name_(name)
{}

daemon::~daemon()
{}

void
daemon::operator()()
{
    if (::daemon(0, 0) == -1) {
        CXXU_SYSDIE("failed to daemonize");
    }

    tv::logger::get().daemon(true, name_);

    auto &s = tv::signaler::get();

    s.register_signal(
        tv::signal::reconfig,
        [this](tv::signal sig) { reconfig(); }
    );
    s.register_signal(
        tv::signal::quit,
        [this](tv::signal sig) { quit(); }
    );
    s.register_signal(
        tv::signal::user1,
        [this](tv::signal sig) { user1(); }
    );
    s.register_signal(
        tv::signal::user2,
        [this](tv::signal sig) { user2(); }
    );

    init();
    run();
}

void
daemon::init()
{}

void
daemon::reconfig()
{}

void
daemon::user1()
{}

void
daemon::user2()
{}

} // namespace cxxu
