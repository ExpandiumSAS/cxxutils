#ifndef __CXXU_DAEMON_H__
#define __CXXU_DAEMON_H__

#include <string>

#include <cxxutils/config.h>

namespace cxxu {

class CXXUTILS_API daemon
{
public:
    daemon(const std::string& name);
    virtual ~daemon();

    void operator()(bool daemonize = true);

private:
    virtual void init();
    virtual void run() = 0;
    virtual void reconfig();
    virtual void quit() = 0;
    virtual void user1();
    virtual void user2();

    std::string name_;
};

} // namespace cxxu

#endif // __CXXU_DAEMON_H__
