/*
 * interfaces.hpp
 *
 *  Created on: Jan 20, 2015
 *      Author: expand
 */

#ifndef INCLUDE_CXXU_CXXU_INTERFACES_HPP_
#define INCLUDE_CXXU_CXXU_INTERFACES_HPP_

#include <string>
#include <vector>

#include <cxxutils/config.h>


namespace cxxu {

class CXXUTILS_API interfaces
{
public:
    struct interface_type {
        std::string     name;
        std::string     address;
    };

    using interface_list_type = std::vector<interface_type>;

public:
    interfaces()
    { find_interfaces(); }

    template<typename Callback>
    void
    for_each(Callback call)
    {
        for (auto& intf : interface_list_) {
            call(intf);
        }
    }

private:
    void find_interfaces();

private:
    interface_list_type     interface_list_;

};

}   // namespace cxxu

#endif /* INCLUDE_CXXU_CXXU_INTERFACES_HPP_ */
