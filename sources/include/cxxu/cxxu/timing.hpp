#ifndef __CXXU_TIMING_H__
#define __CXXU_TIMING_H__

#include <string>
#include <ostream>
#include <chrono>

#include <timevault/config.h>

namespace cxxu {

class TIMEVAULT_API timer
{
public:
    timer(bool s = false);
    ~timer();

    void start();
    void stop();
    void reset(bool s = false);
    void pause();
    void resume();
    float elapsed() const;

private:
    typedef std::chrono::time_point<std::chrono::system_clock> time_point;

    float duration(time_point s, time_point e) const;

    float elapsed_;
    bool paused_;
    bool running_;
    time_point start_;
    time_point stop_;
};

TIMEVAULT_API
std::string
format_duration(float duration);

inline
std::ostream&
operator<<(std::ostream& os, const timer& t)
{
    os << format_duration(t.elapsed());

    return os;
}

template <typename Callable>
float
time(Callable c)
{
    timer t;

    c();

    return t.elapsed();
}

template <typename Callable>
std::string
time_str(Callable c)
{ return format_duration(time(c)); }

} // namespace cxxu

#endif // __CXXU_TIMING_H__
