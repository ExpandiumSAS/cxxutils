#ifndef __CXXU_TASK_SET_H__
#define __CXXU_TASK_SET_H__

#include <mutex>
#include <condition_variable>
#include <deque>
#include <memory>

#include <cxxu/task.hpp>

#include <cxxutils/config.h>

namespace cxxu {

class CXXUTILS_API task_set : public task_base
{
public:
    task_set() noexcept;
    task_set(void_function&& h) noexcept;
    task_set(task_set&& other) noexcept;
    task_set(const task_set& other) noexcept;
    virtual ~task_set();

    template <typename ...Args>
    void
    add_task(Args&&... args)
    {
        auto tptr = new_task(std::forward<Args>(args)...);
        futures_.push_back(tptr->get_future());
        task_ptrs_.push(std::move(tptr));
    }

    virtual void operator()();

private:
    virtual void run() = 0;

protected:
    void_function h_;
    std::deque<task_future> futures_;
    task_ptr_queue task_ptrs_;
    std::mutex m_;
    std::condition_variable cv_;
};

typedef std::unique_ptr<task_set> task_set_ptr;

template <typename TaskSet, typename ...Args>
task_set_ptr
new_task_set(Args&&... args)
{
    return task_set_ptr(new TaskSet(std::forward<Args>(args)...));
}

} // namespace cxxu

#endif // __CXXU_TASK_SET_H__
