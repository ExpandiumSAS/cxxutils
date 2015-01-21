/*
 * interfaces.cpp
 *
 *  Created on: Jan 20, 2015
 *      Author: expand
 */

#include <cxxu/interfaces.hpp>

#ifdef LINUX
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

namespace cxxu {

#ifdef LINUX
void
find_local_interfaces(interfaces::interface_list_type& interfaces)
{
    struct ifaddrs *addrs,*tmp;

    ::getifaddrs(&addrs);
    tmp = addrs;

    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            char host[NI_MAXHOST];

            int s = ::getnameinfo(
                tmp->ifa_addr, sizeof(struct sockaddr_in),
                host, NI_MAXHOST,
                NULL, 0, NI_NUMERICHOST
            );

            if (s != 0) {
                interfaces::interface_type intf;
                intf.name = tmp->ifa_name;
                intf.address = host;
                interfaces.push_back(intf);;
            }
        }

        tmp = tmp->ifa_next;
    }

    ::freeifaddrs(addrs);
}
#endif  // LINUX

#ifdef DARWIN
void
find_local_interfaces(interfaces::interface_list_type& interface)
{

}
#endif  // DARWIN

void
interfaces::find_interfaces()
{
    find_local_interfaces(interface_list_);
}

}   // namespace cxxu


