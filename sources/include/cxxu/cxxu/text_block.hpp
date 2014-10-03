#ifndef __CXXU_TEXT_BLOCK_H__
#define __CXXU_TEXT_BLOCK_H__

#include <ostream>
#include <string>
#include <vector>

#include <cxxutils/config.h>

namespace cxxu {

class CXXUTILS_API text_block
{
public:
    text_block();
    virtual ~text_block();

    void push_line(const std::string& l);
    void clear();

    std::string block() const;
    std::string indented_block() const;

private:
    std::vector<std::string> lines_;
};

inline
std::ostream&
operator<<(std::ostream& os, const text_block& tb)
{
    os << tb.block();

    return os;
}

} // namespace cxxu

#endif // __CXXU_TEXT_BLOCK_H__
