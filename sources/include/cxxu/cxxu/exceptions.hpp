#ifndef __CXXU_EXCEPTIONS_H__
#define __CXXU_EXCEPTIONS_H__

#include <string>
#include <stdexcept>

namespace cxxu {

class exception : public std::runtime_error
{
public:
    explicit exception(const std::string& what)
    : std::runtime_error(what)
    {}

    virtual ~exception()
    {}
};

} // namespace cxxu

#endif // __CXXU_EXCEPTIONS_H__
