#ifndef __CXXU_TYPE_TRAITS_H__
#define __CXXU_TYPE_TRAITS_H__

#include <type_traits>
#include <memory>

namespace cxxu {

template <typename T>
struct is_shared_ptr_helper : std::false_type
{
    typedef T element_type;

    static
    element_type& deref(element_type& e)
    { return e; }

    static
    const element_type& deref(const element_type& e)
    { return e; }
};

template <typename T>
struct is_shared_ptr_helper<std::shared_ptr<T>> : std::true_type
{
    typedef typename std::remove_cv<T>::type element_type;
    typedef std::shared_ptr<element_type> ptr_type;

    static
    element_type& deref(ptr_type& p)
    { return *p; }

    static
    const element_type& deref(const ptr_type& p)
    { return *p; }
};

template <typename T>
struct is_shared_ptr
: is_shared_ptr_helper<typename std::remove_cv<T>::type>
{};

} // namespace cxxu

#endif // __CXXU_TYPE_TRAITS_H__
