#ifndef __CXXU_RW_TX_H__
#define __CXXU_RW_TX_H__

#include <string>
#include <functional>
#include <fstream>

#include <cxxutils/config.h>

namespace cxxu {

using rw_tx_w_cb = std::function<void(std::ofstream& ofs)>;
using rw_tx_r_cb = std::function<void(std::ifstream& ifs)>;

enum class rw_tx_op
{
    truncate,
    append
};

class CXXUTILS_API rw_tx
{
public:
    rw_tx(const std::string& file, rw_tx_op op = rw_tx_op::truncate);
    ~rw_tx();

    bool operator()(rw_tx_w_cb cb);
    bool operator()(rw_tx_r_cb cb);

private:
    std::string file_;
    rw_tx_op op_;
    std::string backup_file_;
    std::string new_file_;
};

} // namespace cxxu

#endif // __CXXU_RW_TX_H__
