#ifndef _CXXU_PARALLEL_FOR_H__
#define _CXXU_PARALLEL_FOR_H__

#include <functional>

#include <cxxu/spawn.hpp>

namespace cxxu {

template <typename Body>
void parallel_for(std::size_t start, std::size_t end, Body b)
{
    cxxu::task_group g;

    for (std::size_t i = start; i < end; ++i) {
        g.add_task(std::bind([b](std::size_t pos){ b(pos); }, i));
    }

    auto f = spawn(std::move(g));
    f.wait();
}

template <typename Body>
void parallel_for(std::size_t count, Body b)
{ parallel_for(0, count, b); }

template <typename Container, typename Body>
void parallel_for(Container& c, Body b)
{}

} // namespace cxxu

#endif // _CXXU_PARALLEL_FOR_H__
