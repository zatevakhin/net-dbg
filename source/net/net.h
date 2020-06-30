
#ifndef LIB_NET_H
#define LIB_NET_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <netinet/ip_icmp.h>   // icmp header
#include <netinet/udp.h>       // udp header
#include <netinet/tcp.h>       // tcp header
#include <netinet/ip.h>        // ip header
#include <netinet/if_ether.h>  // ETH_P_ALL
#include <net/ethernet.h>      // ether_header
#include <arpa/inet.h>

#include "luaapi/luaapi.h"


typedef void (*net_packet_handler)(lua_State* l_state, unsigned char* buffer, ssize_t size);

int net_setup_monitor(net_packet_handler callback, lua_State* l_state);


#endif //LIB_NET_H