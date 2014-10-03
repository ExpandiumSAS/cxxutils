#include <signal.h>

#include <cxxu/signaler.hpp>

namespace cxxu {

void
signal_handler(int sig)
{
    cxxu::signal s = cxxu::signal::none;

    switch (sig) {
        case SIGHUP:
        s = cxxu::signal::reconfig;
        break;
        case SIGINT:
        case SIGQUIT:
        case SIGTERM:
        s = cxxu::signal::quit;
        break;
        case SIGUSR1:
        s = cxxu::signal::user1;
        break;
        case SIGUSR2:
        s = cxxu::signal::user2;
        break;
        default:
        s = cxxu::signal::none;
        break;
    }

    if (s != cxxu::signal::none) {
        signaler::get().handle_signal(s);
    }
}

signaler&
signaler::get()
{
    static signaler s;
    return s;
}

signaler::signaler()
{
    ::signal(SIGHUP, signal_handler);
    ::signal(SIGINT, signal_handler);
    ::signal(SIGQUIT, signal_handler);
    ::signal(SIGTERM, signal_handler);
    ::signal(SIGUSR1, signal_handler);
    ::signal(SIGUSR2, signal_handler);
}

signaler::~signaler()
{}

void
signaler::register_signal(cxxu::signal s, signal_function sf)
{ m_[s].push_back(sf); }

void
signaler::handle_signal(cxxu::signal s)
{
    auto it = m_.find(s);

    if (it != m_.end()) {
        for (auto sf : it->second) {
            sf(s);
        }
    }
}

} // namespace cxxu
