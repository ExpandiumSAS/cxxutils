#include <cxxu/task_chain.hpp>
#include <cxxu/task_runner.hpp>

namespace cxxu {

void
task_chain::run()
{
    while (task_ptrs_.size() > 0) {
        auto f = std::move(futures_.front());
        futures_.pop_front();
        task_runner::get()(std::move(task_ptrs_.front()));
        task_ptrs_.pop();
        f.get();
    }
}

} // namespace cxxu
