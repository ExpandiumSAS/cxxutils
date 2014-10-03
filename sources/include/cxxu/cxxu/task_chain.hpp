#ifndef __CXXU_TASK_CHAIN_H__
#define __CXXU_TASK_CHAIN_H__

#include <cxxu/task_set.hpp>

#include <cxxutils/config.h>

namespace cxxu {

class CXXUTILS_API task_chain : public task_set
{
private:
    virtual void run();
};

template <typename ...Args>
task_set_ptr
new_task_chain(Args&&... args)
{
    return new_task_set<task_chain>(std::forward<Args>(args)...);
}

} // namespace cxxu

#endif // __CXXU_TASK_CHAIN_H__
