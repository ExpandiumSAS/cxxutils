#include <sstream>
#include <iostream>
#include <string>
#include <cctype>
#include <boost/format.hpp>

#include <cxxu/dump.hpp>

namespace cxxu {

const unsigned int bytes_per_line = 16;

void hex_dump_line(
    std::ostream& os,
    unsigned int line,
    const unsigned char* b,
    unsigned int count
)
{
    if (count == 0) {
        return;
    }

    std::ostringstream hoss;
    std::ostringstream aoss;

    for (unsigned int i = 0; i < bytes_per_line && count > 0; i++) {
        if (i % 2 == 0) {
            hoss << " ";
        }

        if (i == bytes_per_line / 2) {
            hoss << " ";
        }

        hoss << boost::format("%02x") % (unsigned int) b[i];
        aoss << boost::format("%c") % (unsigned char) (std::isprint((int) b[i]) ? b[i] : '.');

        count--;
    }

    os
        << boost::format("%07x:") % (line * bytes_per_line)
        << boost::format("%-40s  %-16s") % hoss.str() % aoss.str() << "\n";
}

void hex_dump(std::ostream& os, const void* b, unsigned int count)
{

    os
        << "hex dump of " << count << " bytes"
        << " from " << boost::format("%p") % b
        << " to " << boost::format("%p") % (void*) ((unsigned char*) b + count) << "\n";

    unsigned int lines = count / bytes_per_line;
    unsigned int l;
    unsigned char* bc = (unsigned char*) b;

    for (l = 0; l < lines; l++) {
        hex_dump_line(os, l, bc, count);
        bc += bytes_per_line;
        count -= bytes_per_line;
    }

    hex_dump_line(os, l, bc, count);
}

void hex_dump(const void* b, unsigned int count)
{
    hex_dump(std::cout, b, count);
}

void hex_dump(const std::string& s)
{
    hex_dump(std::cout, (const void*) s.c_str(), s.size());
}

} // namespace cxxu
