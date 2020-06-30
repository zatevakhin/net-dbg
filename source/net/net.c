
#include "net.h"
#include "macro/dbg.h"
#include "luaapi/luaapi.h"


int net_setup_monitor(net_packet_handler callback, lua_State* l_state)
{
    const size_t buffer_size = IP_MAXPACKET + 1;
    unsigned char *buffer = (unsigned char *)malloc(buffer_size);

    if (0 == buffer)
    {
        trc_error("Can't create buffer.");
        return -1;
    }

    int fd = socket(AF_PACKET , SOCK_RAW , htons(ETH_P_ALL));

    if (fd < 0)
    {
        trc_error("Socket error.");
        return -1;
    }

    struct sockaddr sockaddr_obj;
    size_t sockaddr_size = sizeof(sockaddr_obj);
    ssize_t buffer_data_size = 0;

    for (;;)
    {
        // Receive packet
        buffer_data_size = recvfrom(fd , buffer , buffer_size , 0, &sockaddr_obj , (socklen_t*)&sockaddr_size);
        
        if (buffer_data_size < 0)
        {
            trc_error("recvfrom error, failed to get packets");
            return -1;
        }

        // Handle packet
        callback(l_state, buffer, buffer_data_size);
    }
    close(fd);

    return 0;
}