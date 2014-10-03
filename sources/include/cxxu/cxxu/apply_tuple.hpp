#ifndef __CXXU_APPLY_TUPLE_H__
#define __CXXU_APPLY_TUPLE_H__

//
// Copied and (slightly adapted) from:
//
// https://www.preney.ca/paul/archives/1099
//

#include <cstddef>
#include <type_traits>

#undef RETURNS
#define RETURNS(x) -> decltype(x) { return x; }

namespace cxxu {

namespace detail {

template <std::size_t...>
struct indices
{};

template <
    std::size_t Begin,
    std::size_t End,
    typename Indices = indices<>,
    typename Enable = void
>
struct make_seq_indices
{
    static_assert(Begin <= End, "Begin must be <= End");
};

template <
    std::size_t Begin,
    std::size_t End,
    template <std::size_t...> class I,
    std::size_t... Indices
>
struct make_seq_indices<
    Begin, End,
    I<Indices...>,
    typename std::enable_if<Begin < End, void>::type
>
{
    using type =
        typename make_seq_indices<
            Begin + 1, End,
            I<Indices..., Begin>
        >::type
        ;
};

template <
    std::size_t Begin,
    std::size_t End,
    typename Indices
>
struct make_seq_indices<
    Begin, End,
    Indices,
    typename std::enable_if<Begin == End, void>::type
>
{
    using type = Indices;
};

template <std::size_t Begin, std::size_t End>
using make_seq_indices_T =
    typename make_seq_indices<Begin, End>::type
    ;

template <
    typename F,
    typename Tuple,
    template <std::size_t...> class I,
    std::size_t... Indices
>
inline constexpr auto apply_tuple(
    F&& f,
    Tuple&& t,
    I<Indices...>&&
)
RETURNS(
    std::forward<F>(f)(
        std::get<Indices>(std::forward<Tuple>(t))...
    )
);

} // namespace detail

template <typename F, typename... Args>
inline constexpr auto apply(F&& f, Args&&... args)
RETURNS(std::forward<F>(f)(std::forward<Args>(args)...));

template <
    typename F,
    typename Tuple,
    typename Indices =
        detail::make_seq_indices_T<
            0,
            std::tuple_size<typename std::decay<Tuple>::type>::value
        >
>
inline constexpr auto apply_tuple(F&& f, Tuple&& t)
RETURNS(
    detail::apply_tuple(
        std::forward<F>(f),
        std::forward<Tuple>(t),
        Indices{}
    )
);

} // namespace cxxu

#undef RETURNS

#endif // __CXXU_APPLY_TUPLE_H__
