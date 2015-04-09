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
daemon::operator()(bool daemonize)
{
    if (daemonize) {
        if (::daemon(0, 0) == -1) {
            CXXU_SYSDIE("failed to daemonize");
        }

        cxxu::logger::get().daemon(true, name_);
    }

    auto &s = cxxu::signaler::get();

    s.register_signal(
        cxxu::signal::reconfig,
        [this](cxxu::signal sig) { reconfig(); }
    );
    s.register_signal(
        cxxu::signal::quit,
        [this](cxxu::signal sig) { quit(); }
    );
    s.register_signal(
        cxxu::signal::user1,
        [this](cxxu::signal sig) { user1(); }
    );
    s.register_signal(
        cxxu::signal::user2,
        [this](cxxu::signal sig) { user2(); }
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
