#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <type_traits>

#include <cxxu/rw_tx.hpp>

#include <cxxu/utils.hpp>
#include <cxxu/logging.hpp>

namespace cxxu {

rw_tx::rw_tx(const std::string& file, rw_tx_op op)
: file_(file),
op_(op),
backup_file_(file_ + ".bak"),
new_file_(file_ + ".new")
{}

rw_tx::~rw_tx()
{}

bool
rw_tx::operator()(rw_tx_w_cb cb)
{
    try {
        cxxu::mkfilepath(file_);

        auto mode = std::ios::trunc;

        if (cxxu::file_exists(file_)) {
            if (op_ == rw_tx_op::append) {
                cxxu::copy_file(file_, new_file_);
                mode = std::ios::app;
            }
        }

        std::ofstream ofs(new_file_, mode);

        if (!ofs.is_open()) {
            throw std::runtime_error("failed to open " + new_file_);
        }

        cb(ofs);

        ofs << std::flush;
        ofs.close();

        if (cxxu::file_exists(file_)) {
            cxxu::rename(file_, backup_file_);
        }

        cxxu::rename(new_file_, file_);
        cxxu::rmfile(backup_file_);
    } catch (const std::exception& e) {
        if (cxxu::file_exists(backup_file_)) {
            cxxu::rename(backup_file_, file_);
        }

        cxxu::rmfile(new_file_);

        error()
            << "failed to save new file "
            << new_file_ << ": " << e.what()
            ;
    }

    return true;
}

bool
rw_tx::operator()(rw_tx_r_cb cb)
{
    bool has_file = cxxu::file_exists(file_);
    bool has_backup_file = cxxu::file_exists(backup_file_);

    cxxu::rmfile(new_file_);

    if (has_file) {
        cxxu::rmfile(backup_file_);
    } else if (has_backup_file) {
        log() << "restoring " << file_ << " from backup";

        try {
            cxxu::rename(backup_file_, file_);
            has_file = true;
            log() << file_ << " restored from backup";
        } catch (...) {
            error()
                << "WHOA ! Failed to restore "
                << file_ << " from backup"
                ;

            throw;
        }
    }

    if (!has_file) {
        // Nothing to read
        return true;
    }

    try {
        std::ifstream ifs(file_);

        if (!ifs.is_open()) {
            throw std::runtime_error("failed to open " + file_);
        }

        cb(ifs);
    } catch (const std::exception& e) {
        log()
            << "failed to read file "
            << file_ << ": " << e.what()
            ;

        return false;
    }

    return true;
}

} // namespace cxxu
