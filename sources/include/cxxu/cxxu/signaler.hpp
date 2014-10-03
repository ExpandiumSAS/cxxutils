#ifndef __CXXU_SIGNALER_H__
#define __CXXU_SIGNALER_H__

#include <functional>
#include <vector>
#include <map>

#include <timevault/config.h>

namespace cxxu {

void
signal_handler(int sig);

enum class signal
{
    none,
    reconfig,
    quit,
    user1,
    user2
};

typedef std::function<void(tv::signal)> signal_function;

class TIMEVAULT_API signaler
{
public:
    static signaler& get();

    void register_signal(tv::signal s, signal_function sf);

private:
    friend void signal_handler(int sig);

    typedef std::vector<signal_function> signal_functions;
    typedef std::map<tv::signal, signal_functions> signal_map;

    signaler();
    ~signaler();

    void handle_signal(tv::signal s);

    signal_map m_;
};

} // namespace cxxu

#endif // __CXXU_SIGNALER_H__
