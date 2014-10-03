#include <cxxu/timing.hpp>

namespace cxxu {

timer::timer(bool s)
{ reset(s); }

timer::~timer()
{}

void
timer::reset(bool s)
{
    elapsed_ = 0.0;
    paused_ = false;
    running_ = false;

    if (s) {
        start();
    }
}

void
timer::start()
{
    if (running_ || paused_) {
        return;
    }

    start_ = std::chrono::system_clock::now();
    running_ = true;
}

void
timer::stop()
{
    if (!running_ || paused_) {
        return;
    }

    stop_ = std::chrono::system_clock::now();
}

void
timer::pause()
{
    if (paused_ || !running_) {
        return;
    }

    elapsed_ += elapsed();
    paused_ = true;
}

void
timer::resume()
{
    if (!paused_ || !running_) {
        return;
    }

    paused_ = false;
    start();
}

float
timer::elapsed() const
{
    float e = 0.0;

    if (paused_) {
        e = elapsed_;
    } else if (running_) {
        e = duration(start_, std::chrono::system_clock::now());
    } else {
        e = duration(start_, stop_);
    }

    return e;
}

float
timer::duration(time_point s, time_point e) const
{
    std::chrono::duration<float> secs = e - s;

    return secs.count() + elapsed_;
}

std::string
format_duration(float duration)
{
    char str[20];
    std::snprintf(str, 20, "%.5f", duration);

    return std::string(str);
}

} // namespace cxxu
