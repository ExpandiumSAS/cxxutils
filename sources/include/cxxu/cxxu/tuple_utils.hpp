#ifndef __CXXU_TUPLE_UTILS_H__
#define __CXXU_TUPLE_UTILS_H__

#include <stdint.h>

#include <type_traits>
#include <tuple>

#define RETURNS(x) -> decltype(x) { return x; }

namespace cxxu {

template <int ...>
struct tuple_indices
{};

template <int N, int ...I>
struct make_tuple_indices
: make_tuple_indices<N - 1, N - 1, I...>
{};

template<int ...I>
struct make_tuple_indices<0, I...> {
    typedef tuple_indices<I...> type;
};

namespace detail {

template <std::size_t I, class Tuple, typename F, typename ...Args>
struct for_each_impl {
    static void for_each(const Tuple& t, F& f, Args&&... args)
    {
        for_each_impl<I - 1, Tuple, F, Args...>::for_each(t, f, std::forward<Args>(args)...);
        f(I, std::get<I>(t), std::forward<Args>(args)...);
    }
};

template <class Tuple, typename F, typename... Args>
struct for_each_impl<0, Tuple, F, Args...> {
    static void for_each(const Tuple& t, F& f, Args&&... args)
    {
        f(0, std::get<0>(t), std::forward<Args>(args)...);
    }
};

template<std::size_t I, class Tuple, class Params, typename F>
struct for_each_with_impl {
    static void for_each_with(const Tuple& t, Params& p, const F& f) {
        for_each_with_impl<I - 1, Tuple, Params, F>::for_each_with(t, p, f);
        f(I, std::get<I>(t), std::get<I>(p));
    }
};

template<class Tuple, class Params, typename F>
struct for_each_with_impl<0, Tuple, Params, F> {
    static void for_each_with(const Tuple& t, Params& p, const F& f) {
        f(0, std::get<0>(t), std::get<0>(p));
    }
};

template <size_t N>
struct apply {
    template <typename F, typename... ArgsT, typename... Args>
    static inline auto apply_tuple(
        const F& f,
        std::tuple<ArgsT...>& t,
        Args&... args
    )
    RETURNS(apply<N - 1>::apply_tuple(f, t, std::get<N - 1>(t), args...));
};

template <>
struct apply<0> {
    template <typename F, typename... ArgsT, typename... Args>
    static inline auto apply_tuple(
        const F& f,
        std::tuple<ArgsT...>&,
        Args&... args
    )
    RETURNS(f(args...));
};

} // namespace detail

template<class Tuple, typename F, typename... Args>
void for_each(const Tuple& t, F& f, Args&&... args)
{
    detail::for_each_impl<
        std::tuple_size<Tuple>::value - 1,
        Tuple, F, Args...
    >::for_each(t, f, std::forward<Args>(args)...);
}

template<class Tuple, class ParamsTuple, typename F>
F
for_each_with(const Tuple& t, ParamsTuple& p, const F& f)
{
    detail::for_each_with_impl<
        std::tuple_size<Tuple>::value - 1,
        Tuple, ParamsTuple, F
    >::for_each_with(t, p, f);

    return f;
}

template <typename F, typename... ArgsT>
static inline
auto apply_tuple(const F& f, std::tuple<ArgsT...>& t)
RETURNS(detail::apply<sizeof...(ArgsT)>::apply_tuple(f, t));

} // namespace cxxu

#endif // __CXXU_TUPLE_UTILS_H__
