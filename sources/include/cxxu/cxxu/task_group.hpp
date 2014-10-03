#ifndef __CXXU_TASK_GROUP_H__
#define __CXXU_TASK_GROUP_H__

#include <vector>

#include <cxxu/task_set.hpp>

#include <cxxutils/config.h>

namespace cxxu {

class CXXUTILS_API task_group : public task_set
{
private:
    virtual void run();
    void task_done(std::size_t pos);
    void clean_task(std::size_t pos);
    bool all_done();

    std::size_t task_count_;
    std::vector<bool> task_done_;
    std::vector<bool> task_cleaned_;
};

template <typename ...Args>
task_set_ptr
new_task_group(Args&&... args)
{
    return new_task_set<task_group>(std::forward<Args>(args)...);
}

} // namespace cxxu

#endif // __CXXU_TASK_GROUP_H__
