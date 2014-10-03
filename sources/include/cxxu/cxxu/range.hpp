#ifndef __CXXU_RANGE_H__
#define __CXXU_RANGE_H__

namespace cxxu {

//
// Copied and slightly adapted from:
//
// http://cplusplusmusings.wordpress.com/2013/04/14/range-based-for-loops-and-pairs-of-iterators/
//

template <typename Iterator>
struct range
{
    range(Iterator first, Iterator last)
    : f_(first),
    l_ (last)
    {}

    Iterator begin() const
    { return f_; }

    Iterator end() const
    { return l_; }

private:
    Iterator f_;
    Iterator l_;
};

template <typename Iterator>
range<Iterator>
make_range(Iterator f, Iterator l)
{
    return range<Iterator>(f, l);
}

} // namespace cxxu

#endif // __CXXU_RANGE_H__
