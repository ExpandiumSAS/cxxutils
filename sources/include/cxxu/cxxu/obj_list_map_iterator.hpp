#ifndef __CXXU_OBJ_LIST_MAP_ITERATOR_H__
#define __CXXU_OBJ_LIST_MAP_ITERATOR_H__

#include <type_traits>

#include <boost/iterator/iterator_facade.hpp>

#include <cxxu/utils.hpp>
#include <cxxu/type_traits.hpp>

namespace cxxu {

template <typename C>
struct obj_list_map_element
{
    typedef typename cxxu::extract_value<C>::type value_type;
    typedef is_shared_ptr<value_type> is_shared_ptr_type;

    typedef typename is_shared_ptr_type::element_type element_type;

    typedef typename std::conditional<
        std::is_const<C>::value,
        typename std::add_const<element_type>::type,
        element_type
    >::type type;
};

template <typename C>
class obj_list_map_iterator
: public boost::iterator_facade<
    obj_list_map_iterator<C>,
    // Value returned
    typename obj_list_map_element<C>::type,
    // Type of iterator
    boost::bidirectional_traversal_tag
>
{
public:
    typedef typename obj_list_map_element<C>::type value_type;
    typedef typename obj_list_map_element<C>::is_shared_ptr_type is_shared_ptr_type;
    typedef value_type& reference;
    typedef typename cxxu::extract_iterator<C>::type iterator;

    obj_list_map_iterator()
    : it_()
    {}

    obj_list_map_iterator(iterator it)
    : it_(it)
    {}

    obj_list_map_iterator(const obj_list_map_iterator& other)
    : it_(other.it_)
    {}

    virtual ~obj_list_map_iterator()
    {}

private:
    friend class boost::iterator_core_access;
    template <typename> friend class obj_list_map_iterator;

    void increment()
    { ++it_; }

    void decrement()
    { --it_; }

    bool equal(const obj_list_map_iterator& other) const
    { return it_ == other.it_; }

    reference dereference() const
    { return is_shared_ptr_type::deref(*it_); }

    iterator it_;
};

} // namespace cxxu

#endif // __CXXU_OBJ_LIST_MAP_ITERATOR_H__
