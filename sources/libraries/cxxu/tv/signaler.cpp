#include <signal.h>

#include <cxxu/signaler.hpp>

namespace cxxu {

void
signal_handler(int sig)
{
    tv::signal s = tv::signal::none;

    switch (sig) {
        case SIGHUP:
        s = tv::signal::reconfig;
        break;
        case SIGINT:
        case SIGQUIT:
        case SIGTERM:
        s = tv::signal::quit;
        break;
        case SIGUSR1:
        s = tv::signal::user1;
        break;
        case SIGUSR2:
        s = tv::signal::user2;
        break;
        default:
        s = tv::signal::none;
        break;
    }

    if (s != tv::signal::none) {
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
signaler::register_signal(tv::signal s, signal_function sf)
{ m_[s].push_back(sf); }

void
signaler::handle_signal(tv::signal s)
{
    auto it = m_.find(s);

    if (it != m_.end()) {
        for (auto sf : it->second) {
            sf(s);
        }
    }
}

} // namespace cxxu
