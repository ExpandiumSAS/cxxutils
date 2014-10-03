#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/md5.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <cxxu/utils.hpp>

#ifdef UNIX
#include <unistd.h>
#include <cstdio>
#include <cxxabi.h>
#include <glob.h>
#endif

namespace cxxu {

const std::size_t progress_ratio_width = 3;
const std::size_t progress_bar_chars = 4;
const std::size_t progress_bar_extra_width = progress_ratio_width + progress_bar_chars;

bool
interactive(void)
{
#ifdef UNIX
    return (
        ::isatty(::fileno(::stdin)) == 1
        &&
        ::isatty(::fileno(::stdout)) == 1
        &&
        (std::getenv("HARNESS_ACTIVE") == nullptr)
    );
#endif

#ifdef WINDOWS
    // TODO: under windows there should be a _isatty or similar...
    return true;
#endif
}

void set_progress(std::size_t x, std::size_t n, std::size_t w)
{
    if (!interactive()) {
        return;
    }

    float ratio = x / (float) n;
    std::size_t c = ratio * w;

    std::cout << "\r" << std::flush;

    std::cout << std::setw(3) << (std::size_t) (ratio * 100) << "% [";

    for (std::size_t x = 0; x < c; x++) {
        std::cout << "=";
    }

    for (std::size_t x = c; x < w; x++) {
        std::cout << " ";
    }

    std::cout << "]" << std::flush;
}

void clear_progress(std::size_t w)
{
    if (!interactive()) {
        return;
    }

    for (std::size_t x = 0; x < w + progress_bar_extra_width; x++) {
        std::cout << " ";
    }

    std::cout << "\r" << std::flush;
}

std::string
home_directory(void)
{
    const char* home;
    std::string home_dir;

#ifdef UNIX
    home = getenv("HOME");

    if (home != NULL) {
        home_dir = home;
    }
#endif

#ifdef WINDOWS
    home = getenv("USERPROFILE")

    if (home == NULL) {
        const char* hdrive = getenv("HOMEDRIVE");
        const char* hpath = getenv("HOMEPATH");

        if (hdrive != NULL && hpath != NULL) {
            home_dir = hdrive;
            home_dir += hpath;
        }
    } else {
        home_dir = home;
    }
#endif

    if (home_dir.size() == 0) {
        throw std::runtime_error("failed to find home directory");
    }

    return home_dir;
}

bool
exists(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    return fs::exists(ppath);
}

bool
directory_exists(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    return (
        fs::exists(ppath)
        &&
        fs::is_directory(ppath)
    );
}

bool
file_exists(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    return (
        fs::exists(ppath)
        &&
        fs::is_regular_file(ppath)
    );
}

bool
touch_file(const std::string& path)
{
    if (!mkfilepath(path)) {
        return false;
    }

    std::filebuf fbuf;

    fbuf.open(
        path,
        std::ios_base::in
        | std::ios_base::out
        | std::ios_base::app
        | std::ios_base::binary
    );

    if (!fbuf.is_open()) {
        return false;
    }

    fbuf.close();

    return true;
}

bool
truncate_file(const std::string& path, std::size_t size)
{
    if (!touch_file(path)) {
        return false;
    }

    if (truncate(path.c_str(), size) != 0) {
        return false;
    }

    return true;
}

bool
copy_file(const std::string& from, const std::string& to)
{
    namespace fs = boost::filesystem;

    if (!file_exists(from)) {
        return false;
    }

    std::string to_;

    if (directory_exists(to)) {
        // New file in existing directory
        to_ = fullpath(to, basename(from));
    } else {
        to_ = to;
    }

    std::ifstream source(from, std::ios::binary);
    std::ofstream dest(to_, std::ios::binary | std::ios::trunc);

    if (!(source.is_open() && dest.is_open())) {
        return false;
    }

    dest << source.rdbuf();

    source.close();
    dest.close();

    return true;
}

uint64_t
file_size(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    return fs::file_size(ppath);
}

bool
mkpath(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    if (!fs::exists(ppath)) {
        return fs::create_directories(ppath);
    }

    return true;
}

unsigned long
rmpath(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    return fs::remove_all(ppath);
}

bool
rmfile(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    return fs::remove_all(ppath);
}

bool
mkfilepath(const std::string& path)
{
    bool ret = true;
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    ppath.normalize();

    if (!ppath.has_filename()) {
        throw std::runtime_error("path has no filename");
    }

    fs::path parent = ppath.parent_path();
    fs::path filename = ppath.filename();

    if (!fs::exists(parent)) {
        ret = fs::create_directories(parent);
    }

    return ret;
}

void
rename(const std::string& from_path, const std::string& to_path)
{
    namespace fs = boost::filesystem;
    fs::path from_ppath(from_path);
    fs::path to_ppath(to_path);

    fs::rename(from_ppath, to_ppath);
}

std::string
fullpath(const std::string& rel)
{
    namespace fs = boost::filesystem;
    fs::path ppath(rel);

    return fs::complete(ppath).string();
}

std::string
fullpath(const std::string& dir, const std::string& file)
{
    namespace fs = boost::filesystem;
    fs::path ppath(dir);

    ppath.normalize();
    ppath /= file;

    return ppath.normalize().string();
}

std::string
basename(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    ppath.normalize();

    return ppath.filename().string();
}

std::string
dirname(const std::string& path)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    ppath.normalize();

    return ppath.parent_path().string();
}

unsigned long long
human_readable_size(const std::string& expr)
{
    using namespace boost::xpressive;
    sregex sizeexpr = sregex::compile("(\\d+)(?:(K|M|G))?");
    smatch what;
    unsigned long long unit = 1;
    unsigned long long size = 0;

    if (regex_match(expr, what, sizeexpr)) {
        size = to_num<unsigned long long>(what[1]);

        if (what[2] == "K") {
            unit = 1024;
        } else if (what[2] == "M") {
            unit = 1024 * 1024;
        } else if (what[2] == "G") {
            unit = 1024 * 1024 * 1024;
        }

        size *= unit;
    } else {
        throw std::runtime_error("invalid size: " + expr);
    }

    return size;
}

void
split(const std::string& re, const std::string& expr, tv::string_list& list)
{
    using namespace boost::xpressive;

    list.clear();

    sregex delim = sregex::compile(re);

    sregex_token_iterator cur(expr.begin(), expr.end(), delim, -1);
    sregex_token_iterator end;

    for( ; cur != end; ++cur ) {
        list.push_back(*cur);
    }

    if (list.empty() && expr.size() > 0) {
        list.push_back(expr);
    }
}

void
chomp(std::string& s)
{
    std::string::size_type pos = s.find_last_not_of("\r\n");

    if (pos != std::string::npos) {
        s = s.substr(0, pos + 1);
    } else if (s.size() > 0) {
        s.clear();
    }
}

void
unquote(std::string& s)
{
    boost::algorithm::trim_if(s, boost::algorithm::is_any_of("\""));
}

tv::string_list
glob(const std::string& expr)
{
    glob_t g;
    tv::string_list patterns = split("\\s+", expr);
    tv::string_list matches;
    int flags = 0;

    if (!patterns.empty()) {
        for (const auto& pattern : patterns) {
            glob(pattern.c_str(), flags, NULL, &g);

            // Calling with GLOB_APPEND the first time segfaults...
            // (glob() surely tries to free unallocated data)
            flags |= GLOB_APPEND;
        }

        if (g.gl_pathc > 0) {
            for (size_t i = 0; i < g.gl_pathc; i++) {
                matches.push_back(g.gl_pathv[i]);
            }
        }

        globfree(&g);
    }

    return matches;
}

bool
match(const std::string& expr, const std::string& re)
{
    using namespace boost::xpressive;
    sregex sre = sregex::compile(re);

    return regex_match(expr, sre);
}

bool
extract_delimited(
    std::string& from,
    std::string& to,
    unsigned char delim,
    unsigned char quote
)
{
    to.clear();

    // Find the first semicolon not surrounded by the quote char
    // (if specified)
    unsigned int quote_count = 0;
    bool found = false;
    std::string::iterator iter;

    for (iter = from.begin(); iter != from.end(); ++iter) {
        if (quote != '\0' && *iter == quote) {
            quote_count++;
        } else if (*iter == delim && (quote_count % 2 == 0)) {
            // Found it
            found = true;
            break;
        }
    }

    if (found) {
        to.assign(from.begin(), iter);

        // Remove to from input
        from.erase(from.begin(), iter + 1);
    }

    return found;
}

std::string
demangle_type_name(const std::string& mangled)
{
#if defined(UNIX)
    char* buffer;
    int status;

    buffer = abi::__cxa_demangle(mangled.c_str(), 0, 0, &status);

    if (status == 0) {
        std::string n(buffer);
        free(buffer);

        return n;
    } else {
        return std::string("demangle failure");
    }
#elif defined(WINDOWS)
    return mangled;
#else
    return std::string("unsupported");
#endif
}

std::string
md5sum(const std::string& file)
{
    std::ifstream in(file, std::ios::binary);
    boost::md5 md5(in);

    return md5.digest().hex_str_value();
}

std::string
escape(const char c)
{
    std::string esc;

    if (' ' <= c && c <= '~' && c != '\\' && c != '\'') {
        esc += c;
    } else {
        esc += '\\';

        switch (c) {
            case '\'':  esc += '\'';  break;
            case '\\': esc += '\\'; break;
            case '\t': esc += 't';  break;
            case '\r': esc += 'r';  break;
            case '\n': esc += 'n';  break;
            case '\0':  esc += '0';  break;
            default:
            char const* const hexdig = "0123456789ABCDEF";
            esc += 'x';
            esc += hexdig[c >> 4];
            esc += hexdig[c & 0xF];
        }
    }

    return std::move(esc);
}

std::string
escape_for_string(const char c)
{
    std::string esc;

    if (' ' <= c && c <= '~' && c != '\\' && c != '"') {
        esc += c;
    } else {
        esc += '\\';

        switch (c) {
            case '"':  esc += '"';  break;
            case '\\': esc += '\\'; break;
            case '\t': esc += 't';  break;
            case '\r': esc += 'r';  break;
            case '\n': esc += 'n';  break;
            default:
            char const* const hexdig = "0123456789ABCDEF";
            esc += 'x';
            esc += hexdig[c >> 4];
            esc += hexdig[c & 0xF];
        }
    }

    return std::move(esc);
}

std::string
escape(const std::string& s)
{
    std::string esc;

    for (const auto c : s) {
        esc += escape_for_string(c);
    }

    return std::move(esc);
}

} // namespace cxxu
