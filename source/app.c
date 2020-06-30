
#include "net/net.h"
#include "macro/dbg.h"
#include "macro/macro.h"
#include "luaapi/luaapi.h"


void handler_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);
void icmp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);
void tcp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);
void udp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);

void luaapi_push_eth_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);
void luaapi_push_ip_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);
void luaapi_push_icmp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);
void luaapi_push_tcp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);
void luaapi_push_udp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size);


int main(int argc, char const *argv[])
{
    lua_State *l_state = luaL_newstate();
    luaL_openlibs(l_state);
    luaL_dofile(l_state, "objdump.lua");

    lua_newtable(l_state);
    lua_setglobal(l_state, "packet");

    luaL_dofile(l_state, "main.lua");

    if (net_setup_monitor(&handler_packet, l_state) < 0)
    {
        trc_error("net monitor error.");
    }

    lua_close(l_state);
    return 0;
}

void handler_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));

    if (0 == iph)
    {
        trc_warning("Empty packet");
        return;
    }

    switch (iph->protocol)
    {
        case IPPROTO_ICMP:
            icmp_packet(l_state, buffer, size);
            break;

        // case IPPROTO_IGMP:
        case IPPROTO_TCP:
            tcp_packet(l_state, buffer, size);
            break;

        case IPPROTO_UDP:
            udp_packet(l_state, buffer, size);
            break;
        default:
            trc_warning("Unhandled protocol: %d", iph->protocol);
    }
}


void icmp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    lua_getglobal(l_state, "packet");
    lua_pushstring(l_state, "icmp");
    lua_gettable(l_state, -2);

    luaapi_push_eth_packet(l_state, buffer, size);
    luaapi_push_ip_packet(l_state, buffer, size);
    luaapi_push_icmp_packet(l_state, buffer, size);

    if (lua_pcall(l_state, 3, 0, 0) != 0) {
            trc_error("error in function 'packet.icmp': %s", lua_tostring(l_state, -1));
    }
}

void luaapi_push_eth_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    struct ethhdr *eth = (struct ethhdr *)buffer;

    lua_newtable(l_state);
        lua_newtable(l_state);
        for (int i = 0; i < 6; i++) {
            lua_pushinteger(l_state, eth->h_dest[i]);
            lua_rawseti(l_state, -2, i + 1);
        }
        lua_setfield(l_state, -2, "dest");

        lua_newtable(l_state);
        for (int i = 0; i < 6; i++) {
            lua_pushnumber(l_state, eth->h_source[i]);
            lua_rawseti(l_state, -2, i + 1);
        }
        lua_setfield(l_state, -2, "source");

        lua_pushstring(l_state, "protocol");
        lua_pushinteger(l_state, (unsigned short)eth->h_proto);
        lua_settable(l_state, -3);
}


union IPAddress {
    uint32_t i;
    uint8_t a[4];
};



void luaapi_push_ip_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));

    lua_newtable(l_state);
        luaapi_set_field_int(l_state, "tos", iph->tos);
        luaapi_set_field_int(l_state, "tot_len", iph->tot_len);
        luaapi_set_field_int(l_state, "id", iph->id);
        luaapi_set_field_int(l_state, "frag_off", iph->frag_off);
        luaapi_set_field_int(l_state, "ttl", iph->ttl);
        luaapi_set_field_int(l_state, "protocol", iph->protocol);
        luaapi_set_field_int(l_state, "check", iph->check);

        union IPAddress ip;
        ip.i = iph->saddr;

        lua_newtable(l_state);
        for (int i = 0; i < 4; i++) {
            lua_pushinteger(l_state, ip.a[i]);
            lua_rawseti(l_state, -2, i + 1);
        }
        lua_setfield(l_state, -2, "saddr");

        ip.i = iph->daddr;
        lua_newtable(l_state);
        for (int i = 0; i < 4; i++) {
            lua_pushinteger(l_state, ip.a[i]);
            lua_rawseti(l_state, -2, i + 1);
        }
        lua_setfield(l_state, -2, "daddr");
}

void luaapi_push_icmp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdr_len = iph->ihl * 4;

    struct icmphdr *icmph = (struct icmphdr *)(buffer + iphdr_len  + sizeof(struct ethhdr));


    lua_newtable(l_state);
        luaapi_set_field_int(l_state, "type", icmph->type);
        luaapi_set_field_int(l_state, "code", icmph->code);
        luaapi_set_field_int(l_state, "checksum", ntohs(icmph->checksum));

        lua_newtable(l_state);
            luaapi_set_field_int(l_state, "id", ntohs(icmph->un.echo.id));
            luaapi_set_field_int(l_state, "sequence", ntohs(icmph->un.echo.sequence));
        lua_setfield(l_state, -2, "echo");
}

void luaapi_push_tcp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdr_len = iph->ihl * 4;

    struct tcphdr *tcph = (struct tcphdr *)(buffer + iphdr_len + sizeof(struct ethhdr));

    lua_newtable(l_state);
        luaapi_set_field_int(l_state, "source", ntohs(tcph->source));
        luaapi_set_field_int(l_state, "dest", ntohs(tcph->dest));
        luaapi_set_field_int(l_state, "seq", ntohl(tcph->seq));
        luaapi_set_field_int(l_state, "ack_seq", ntohl(tcph->ack_seq));
        lua_newtable(l_state);
            luaapi_set_field_int(l_state, "urg", (unsigned int)tcph->urg);
            luaapi_set_field_int(l_state, "ack", (unsigned int)tcph->ack);
            luaapi_set_field_int(l_state, "psh", (unsigned int)tcph->psh);
            luaapi_set_field_int(l_state, "rst", (unsigned int)tcph->rst);
            luaapi_set_field_int(l_state, "rst", (unsigned int)tcph->rst);
            luaapi_set_field_int(l_state, "syn", (unsigned int)tcph->syn);
            luaapi_set_field_int(l_state, "fin", (unsigned int)tcph->fin);
        lua_setfield(l_state, -2, "flag");
        
        luaapi_set_field_int(l_state, "window", ntohs(tcph->window));
        luaapi_set_field_int(l_state, "check", ntohs(tcph->check));
        luaapi_set_field_int(l_state, "urg_ptr", tcph->urg_ptr);
}

void tcp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    unsigned short iphdr_len;

    struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr));
    iphdr_len = iph->ihl * 4;

    struct tcphdr *tcph = (struct tcphdr *)(buffer + iphdr_len + sizeof(struct ethhdr));
    int header_size =  sizeof(struct ethhdr) + iphdr_len + tcph->doff * 4;


    lua_getglobal(l_state, "packet");
    lua_pushstring(l_state, "tcp");
    lua_gettable(l_state, -2);

    luaapi_push_eth_packet(l_state, buffer, size);
    luaapi_push_ip_packet(l_state, buffer, size);
    luaapi_push_tcp_packet(l_state, buffer, size);

    lua_pushlstring(l_state, buffer + header_size, size - header_size);
    // lua_pushlightuserdata(l_state, buffer + header_size);

    lua_pushinteger(l_state, size - header_size);

    if (lua_pcall(l_state, 5, 0, 0) != 0) {
            trc_error("error in function 'packet.icmp': %s", lua_tostring(l_state, -1));
    }
}

void udp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    unsigned short iphdr_len;

    struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr));
    iphdr_len = iph->ihl * 4;

    struct udphdr *udph = (struct udphdr*)(buffer + iphdr_len + sizeof(struct ethhdr));
    int header_size =  sizeof(struct ethhdr) + iphdr_len + sizeof udph;

    lua_getglobal(l_state, "packet");
    lua_pushstring(l_state, "udp");
    lua_gettable(l_state, -2);

    luaapi_push_eth_packet(l_state, buffer, size);
    luaapi_push_ip_packet(l_state, buffer, size);
    luaapi_push_udp_packet(l_state, buffer, size);

    lua_pushlstring(l_state, buffer + header_size, size - header_size);
    // lua_pushlightuserdata(l_state, buffer + header_size);

    lua_pushinteger(l_state, size - header_size);

    if (lua_pcall(l_state, 5, 0, 0) != 0) {
            trc_error("error in function 'packet.udp': %s", lua_tostring(l_state, -1));
    }
}

void luaapi_push_udp_packet(lua_State *l_state, unsigned char *buffer, ssize_t size)
{
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdr_len = iph->ihl * 4;

    struct udphdr *udph = (struct udphdr*)(buffer + iphdr_len  + sizeof(struct ethhdr));

    lua_newtable(l_state);
        luaapi_set_field_int(l_state, "source", ntohs(udph->source));
        luaapi_set_field_int(l_state, "dest", ntohs(udph->dest));
        luaapi_set_field_int(l_state, "len", ntohs(udph->len));
        luaapi_set_field_int(l_state, "check", ntohs(udph->check));

}