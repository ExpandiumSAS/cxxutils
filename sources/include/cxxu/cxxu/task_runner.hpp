#ifndef __CXXU_TASK_RUNNER_H__
#define __CXXU_TASK_RUNNER_H__

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <functional>
#include <vector>

#include <cxxu/task.hpp>
#include <cxxu/task_group.hpp>
#include <cxxu/task_chain.hpp>

#include <timevault/config.h>

namespace cxxu {

class TIMEVAULT_API task_runner
{
public:
    static task_runner& get();

    template <typename ...Args>
    task_future operator()(Args&&... args)
    {
        auto tptr = new_task(std::forward<Args>(args)...);
        auto future = tptr->get_future();

        (*this)(std::move(tptr));

        return future;
    }

    void operator()(task_ptr&& p);
    task_future operator()(task_group&& tg);
    task_future operator()(task_chain&& tc);
    task_future operator()(task_set_ptr&& sp);

private:
    task_runner();
    ~task_runner();

    task_runner(const task_runner&) = delete;
    void operator=(const task_runner&) = delete;

    void work();

    std::atomic_bool stop_;
    std::vector<std::thread> threads_;
    std::mutex m_;
    std::condition_variable cv_;
    task_ptr_queue task_ptrs_;
};

} // namespace cxxu

#endif // __CXXU_TASK_RUNNER_H__
