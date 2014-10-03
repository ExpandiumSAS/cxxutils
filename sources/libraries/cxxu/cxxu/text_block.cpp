#include <cxxu/text_block.hpp>
#include <cxxu/utils.hpp>

namespace cxxu {

text_block::text_block()
{}

text_block::~text_block()
{}

void
text_block::push_line(const std::string& l)
{
    if (l.size() > 0) {
        lines_.push_back(l);
    }
}

void
text_block::clear()
{ lines_.clear(); }

std::string
text_block::block() const
{ return cxxu::join("\n", lines_); }

std::string
text_block::indented_block() const
{ return cxxu::indent(block()); }


} // namespace cxxu
