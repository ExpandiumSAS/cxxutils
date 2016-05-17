#include <unistd.h>
#include <cxxabi.h>
#include <glob.h>
#include <sys/types.h>
#include <fcntl.h>

#if defined(LINUX)
#include <sys/sendfile.h>
#elif defined(DARWIN)
#include <sys/socket.h>
#include <sys/uio.h>
#endif

#include <ios>
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdexcept>
#include <regex>

#include <boost/filesystem.hpp>
#include <boost/md5.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <cxxu/utils.hpp>
#include <cxxu/logging.hpp>

namespace cxxu {

const std::size_t progress_ratio_width = 3;
const std::size_t progress_bar_chars = 4;
const std::size_t progress_bar_extra_width =
    progress_ratio_width + progress_bar_chars;

const std::size_t kilobytes = 1024;
const std::size_t megabytes = kilobytes * 1024;
const std::size_t gigabytes = megabytes * 1024;
const std::size_t terabytes = gigabytes * 1024;
const std::size_t petabytes = terabytes * 1024;

struct human_size_conv {
    const char* unit;
    std::size_t scale;
};

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

    int src = ::open(from.c_str(), O_RDONLY, 0);

    CXXU_SYSDIE_IF(src == -1, "failed to open file: " << from);

    int dst = ::creat(to_.c_str(), 0644);

    if (dst == -1) {
        ::close(src);
        CXXU_DIE("failed to create file: " << to_);
    }

    auto ret = send_file(dst, src, nullptr, file_size(from));

    ::close(src);
    ::close(dst);

    CXXU_SYSDIE_IF(ret == -1, "failed to send file: " << from);

    return true;
}

bool
write_file(const std::string& file, write_file_cb cb)
{
    auto backup_file = file + ".old";
    auto backup_ok_file = backup_file + ".ok";
    bool backup_ok = false;
    bool backup_needed = file_exists(file);
    auto write_ok_file = file + ".ok";
    bool write_ok = false;

    try {
        mkfilepath(file);
    } catch (const std::exception& e) {
        error()
            << "failed to create path for "
            << file << ": " << e.what()
            ;

        return false;
    }

    if (backup_needed) {
        try {
            rename(file, backup_file);
            backup_ok = touch_file(backup_ok_file);

            if (!backup_ok) {
                throw std::runtime_error("failed to create " + backup_ok_file);
            }
        } catch (const std::exception& e) {
            rmfile(backup_file);
            rmfile(backup_ok_file);

            error()
                << "failed to backup file "
                << file << ": " << e.what()
                ;

            return false;
        }
    }

    try {
        std::ofstream ofs(file, std::ios::trunc);

        if (!ofs.is_open()) {
            throw std::runtime_error("failed to open " + file);
        }

        cb(ofs);
        ofs << std::flush;

        if (!ofs) {
            throw std::runtime_error("failed to write " + file);
        }

        ofs.close();
        write_ok = touch_file(write_ok_file);

        if (!write_ok) {
            throw std::runtime_error("failed to create " + write_ok_file);
        }
    } catch (const std::exception& e) {
        rmfile(file);
        rmfile(write_ok_file);

        error()
            << "failed to save file "
            << file << ": " << e.what()
            ;

        if (backup_needed) {
            log() << "restoring " << file << " from backup";

            try {
                rename(backup_file, file);
                rename(backup_ok_file, write_ok_file);
            } catch (...) {
                error()
                    << "WHOA ! Failed to restore "
                    << file << " from backup"
                    ;

                throw;
            }

            log() << file << " restored from backup";
        }

        return false;
    }

    rmfile(backup_file);
    rmfile(backup_ok_file);

    if (
        !(file_exists(file) && cxxu::file_exists(write_ok_file))
        ||
        file_exists(backup_file)
        ||
        file_exists(backup_ok_file)
    ) {
        warning() << file << " has bizarre state, please check";
    }

    // Phewww....
    return true;
}

bool
read_file(const std::string& file, read_file_cb cb)
{
    bool has_file = file_exists(file);
    auto backup_file = file + ".old";
    bool has_backup = file_exists(backup_file);
    auto backup_ok_file = backup_file + ".ok";
    bool backup_ok = file_exists(backup_ok_file);
    auto read_ok_file = file + ".ok";
    bool read_ok = file_exists(read_ok_file);

    bool can_restore = has_backup && backup_ok;
    bool can_read = has_file && read_ok;

    if (can_read) {
        if (can_restore) {
            log() << "removing stale backup for " << file;
            rmfile(backup_file);
            rmfile(backup_ok_file);
        }
    } else if (can_restore) {
        log() << "restoring " << file << " from backup";

        try {
            rename(backup_file, file);
            rename(backup_ok_file, read_ok_file);
            log() << file << " restored from backup";
        } catch (...) {
            error()
                << "WHOA ! Failed to restore "
                << file << " from backup"
                ;

            throw;
        }
    } else {
        // Can't read, can't restore... What's going on ?
        if (!has_file && !has_backup) {
            // Nothing was saved, nothing to read
            return true;
        } else {
            if (has_file) {
                error()
                    << "cannot read " << file
                    << ", it's not marked as ok"
                    ;
            }

            if (has_backup) {
                error()
                    << "cannot restore " << file
                    << ", backup isn't marked as ok"
                    ;
            }

            return false;
        }
    }

    if (file_exists(file) && file_exists(read_ok_file)) {
        try {
            std::ifstream ifs(file);

            if (!ifs.is_open()) {
                throw std::runtime_error("failed to open " + file);
            }

            cb(ifs);
        } catch (const std::exception& e) {
            log()
                << "failed to read file "
                << file << ": " << e.what()
                ;

            return false;
        }
    }

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

    if (!fs::exists(ppath)) {
        return false;
    }

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
basename(const std::string& path, const std::string& ext)
{
    namespace fs = boost::filesystem;
    fs::path ppath(path);

    ppath.normalize();

    if (ext.empty()) {
        return ppath.filename().string();
    }

    auto bn = ppath.filename().string();
    auto pos = bn.rfind(ext);

    if (
        pos != std::string::npos
        &&
        pos + ext.size() == bn.size()
    ) {
        bn.erase(pos);
    }

    return bn;
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
    static std::vector<human_size_conv> ct = {
        { "K", kilobytes },
        { "M", megabytes },
        { "G", gigabytes },
        { "T", terabytes },
        { "P", petabytes }
    };

    std::regex sizeexpr("(\\d+)(?:(K|M|G|T|P))?");
    std::smatch what;
    unsigned long long scale = 1;
    unsigned long long size = 0;

    if (regex_match(expr, what, sizeexpr)) {
        size = to_num<unsigned long long>(what[1]);

        for (const auto& c : ct) {
            if (what[2] == c.unit) {
                scale = c.scale;
                break;
            }
        }

        size *= scale;
    } else {
        throw std::runtime_error("invalid size: " + expr);
    }

    return size;
}

std::string
human_readable_size(uint64_t size, const std::string& user_unit)
{
    static std::vector<human_size_conv> ct = {
        { "P", petabytes },
        { "T", terabytes },
        { "G", gigabytes },
        { "M", megabytes },
        { "K", kilobytes }
    };

    std::ostringstream oss;
    double value = size;
    const char* unit = "";
    std::size_t scale = 1;

    for (const auto& c : ct) {
        if (size >= c.scale) {
            scale = c.scale;
            unit = c.unit;
            break;
        }
    }

    value /= scale;

    char buf[50 + 1];

    std::snprintf(buf, sizeof(buf), "%.2f", value);

    oss << buf << unit << user_unit;

    return oss.str();
}

void
split(const std::string& re, const std::string& expr, cxxu::string_list& list)
{
    list.clear();

    if (expr.empty()) {
        return;
    }

    std::regex delim(re);

    auto cur = std::sregex_token_iterator(
        expr.begin(), expr.end(),
        delim,
        -1
    );
    auto end = std::sregex_token_iterator();

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

cxxu::string_list
glob(const std::string& expr)
{
    glob_t g;
    cxxu::string_list patterns = split("\\s+", expr);
    cxxu::string_list matches;
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
    std::regex mre(re);

    return regex_match(expr, mre);
}

bool
match(const std::string& expr, const std::string& re, std::smatch& m)
{
    std::regex mre(re);

    return regex_match(expr, m, mre);
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

    return esc;
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

    return esc;
}

std::string
escape(const std::string& s)
{
    std::string esc;

    for (const auto c : s) {
        esc += escape_for_string(c);
    }

    return esc;
}

ssize_t
send_file(int out_fd, int in_fd, off_t* offset, size_t count)
{
#if defined(LINUX)
    return ::sendfile(out_fd, in_fd, offset, count);
#elif defined(DARWIN)
    off_t ofs = offset ? *offset : 0;
    off_t bytes = count;
    auto ret = ::sendfile(in_fd, out_fd, ofs, &bytes, NULL, 0);

    if (ret == -1) {
        return -1;
    }

    if (offset) {
        *offset += bytes;
    }

    return bytes;
#endif
}

} // namespace cxxu
