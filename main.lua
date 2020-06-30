
packet.icmp = function (eth_hdr, ip_hdr, icmp_hdr)
    print("packet.icmp", eth_hdr, ip_hdr, icmp_hdr)
    -- debug.objdump(icmp_hdr)
end

packet.tcp = function (eth_hdr, ip_hdr, tcp_hdr, data, size)
    print("packet.tcp", eth_hdr, ip_hdr, tcp_hdr, string.len(data), size)
    -- debug.objdump(tcp_hdr)
end

packet.udp = function (eth_hdr, ip_hdr, udp_hdr, data, size)
    if udp_hdr.dest == 53 or udp_hdr.source == 53 then
        print("packet.udp", eth_hdr, ip_hdr, udp_hdr, string.len(data), size)
        -- debug.objdump(udp_hdr)
    end
end

function handle_packet(protocol, buff, buff_size)
    print(protocol, buff, buff_size)
end
