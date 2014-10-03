#ifndef __CXXU_SPAWN_H__
#define __CXXU_SPAWN_H__

#include <functional>
#include <type_traits>

#include <cxxu/task_runner.hpp>

namespace cxxu {

namespace detail {

template <typename TaskSet>
task_future
spawn_set(TaskSet&& set, void_function&& h)
{
    set.set_handler(std::forward<void_function>(h));
    return task_runner::get()(std::forward<TaskSet>(set));
}

} // namespace detail

template <typename ...Args>
task_future
spawn(Args&&... args)
{ return task_runner::get()(std::forward<Args>(args)...); }

inline
task_future
spawn(task_group&& g, void_function&& h)
{ return detail::spawn_set(std::forward<task_group>(g), std::forward<void_function>(h)); }

inline
task_future
spawn(task_chain&& c, void_function&& h)
{ return detail::spawn_set(std::forward<task_chain>(c), std::forward<void_function>(h)); }

} // namespace cxxu

#endif // __CXXU_SPAWN_H__
