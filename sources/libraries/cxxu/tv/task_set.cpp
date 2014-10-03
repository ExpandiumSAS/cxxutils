#include <iostream>

#include <cxxu/task_set.hpp>
#include <cxxu/task_runner.hpp>

namespace cxxu {

task_set::task_set() noexcept
: task_base()
{}

task_set::task_set(void_function&& h) noexcept
: task_base()
{ set_handler(std::forward<void_function>(h)); }

task_set::task_set(task_set&& other) noexcept
: task_base(std::forward<task_base>(other)),
futures_(std::move(other.futures_)),
task_ptrs_(std::move(other.task_ptrs_)),
m_(),
cv_()
{}

task_set::task_set(const task_set& other) noexcept
{ throw std::logic_error("task_set is not copyable"); }

task_set::~task_set()
{}

void
task_set::operator()()
{
    try {
        run();
        call_handler();
        set_value();
    } catch (...) {
        set_exception(std::current_exception());
    }

    call_generic_handler();
}

} // namespace cxxu
