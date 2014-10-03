#include <iostream>
#include <chrono>

#include <cxxu/utils.hpp>
#include <cxxu/task_group.hpp>
#include <cxxu/task_runner.hpp>

namespace cxxu {

void
task_group::run()
{
    task_count_ = task_ptrs_.size();
    task_done_.resize(task_count_, false);
    task_cleaned_.resize(task_count_, false);
    std::size_t task_pos = 0;

    while (task_ptrs_.size() > 0) {
        auto tptr = std::move(task_ptrs_.front());
        tptr->set_generic_handler([this,task_pos]() { task_done(task_pos); });
        task_runner::get()(std::move(tptr));
        task_ptrs_.pop();
        task_pos++;
    }

    for ( ; ; ) {
        {
            std::unique_lock<std::mutex> lock(m_);

            if (all_done()) {
                break;
            }

            cv_.wait(lock);
        }
    }
}

void
task_group::task_done(std::size_t pos)
{
    std::unique_lock<std::mutex> lock(m_);

    task_done_[pos] = true;
    cv_.notify_one();
}

void
task_group::clean_task(std::size_t pos)
{
    if (task_cleaned_[pos]) {
        return;
    }

    task_cleaned_[pos] = true;

    try {
        futures_[pos].get();
    } catch (...) {
        set_exception(std::current_exception());
    }
}

bool
task_group::all_done()
{
    std::size_t done_count = 0;

    for (std::size_t i = 0; i < task_done_.size(); i++) {
        if (task_done_[i]) {
            done_count++;
            clean_task(i);
        }
    }

    return (done_count == task_count_);
}

} // namespace cxxu
