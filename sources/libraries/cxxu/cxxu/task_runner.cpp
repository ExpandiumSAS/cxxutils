#include <iostream>
#include <exception>
#include <system_error>

#include <cxxu/task_runner.hpp>
#include <cxxu/system/info.hpp>
#include <cxxu/logging.hpp>

namespace cxxu {

task_runner&
task_runner::get(void)
{
    static task_runner p;
    return p;
}

task_runner::task_runner()
: stop_(false)
{
    cxxu::system::info i;

    for (std::size_t n = 0; n < i.logical_cpus() + 1; n++) {
        threads_.emplace_back(&task_runner::work, this);
    }
}

task_runner::~task_runner()
{
    {
        std::unique_lock<std::mutex> lock(m_);
        stop_ = true;
        cv_.notify_all();
    }

    for (auto& t : threads_) {
        t.join();
    }
}

void
task_runner::operator()(task_ptr&& p)
{
    std::unique_lock<std::mutex> lock(m_);

    task_ptrs_.push(std::forward<task_ptr>(p));

    cv_.notify_one();
}

task_future
task_runner::operator()(task_group&& tg)
{
    auto ptr = new_task_group(std::forward<task_group>(tg));
    return (*this)(std::move(ptr));
}

task_future
task_runner::operator()(task_chain&& tc)
{
    auto ptr = new_task_chain(std::forward<task_chain>(tc));
    return (*this)(std::move(ptr));
}

task_future
task_runner::operator()(task_set_ptr&& sp)
{
    std::unique_lock<std::mutex> lock(m_);

    auto future = sp->get_future();
    task_ptrs_.push(std::forward<task_set_ptr>(sp));

    cv_.notify_one();

    return future;
}

void
task_runner::work()
{
    task_ptr p;

    for ( ; ; ) {
        {
            std::unique_lock<std::mutex> lock(m_);

            for ( ; ; ) {
                if (stop_) {
                    return;
                }

                if (!task_ptrs_.empty()) {
                    p = std::move(task_ptrs_.front());
                    task_ptrs_.pop();
                    break;
                }

                cv_.wait(lock);
            }
        }

        try {
            (*p)();
        } catch (...) {
            cxxu::error() << "a task failed: unhandled exception";

            // Last resort, should be handle by task itself
            p->set_exception(std::current_exception());
        }
    }
}

} // namespace cxxu
