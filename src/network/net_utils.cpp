#include "network/net_utils.h"

#include <memory>

#include <windows.h>
#include <IPHlpApi.h>

namespace network
{

std::string NetUtils::readableMacAddress(unsigned char bytes[], size_t len, char delim)
{
    char buffer[18] = {};
    char *cur = buffer;
    for (auto i = 0; i < len; ++i)
    {
        sprintf(cur, "%x", bytes[i]);
        if (i != 5)
        {
            cur[2] = delim;
            cur += 3;
        }
    }
    return buffer;
}

DWORD NetUtils::findIpByMac(unsigned char mac[6])
{
    //´Óarp±íÀïÕÒ
    ULONG size = sizeof(MIB_IPNETTABLE);
    std::unique_ptr<MIB_IPNETTABLE> arp_table(static_cast<MIB_IPNETTABLE *>(malloc(size)));
    if (GetIpNetTable(arp_table.get(), &size, FALSE) == ERROR_INSUFFICIENT_BUFFER)
    {
        arp_table.reset(static_cast<MIB_IPNETTABLE *>(malloc(size)));
    }
    GetIpNetTable(arp_table.get(), &size, FALSE);

    DWORD ip = 0;
    for (auto i = 0; i < arp_table->dwNumEntries; ++i)
    {
        auto temp = readableMacAddress(arp_table->table[i].bPhysAddr, arp_table->table[i].dwPhysAddrLen);
        if (memcmp(mac, arp_table->table[i].bPhysAddr, 6) == 0)
        {
            ip = arp_table->table[i].dwAddr;
            break;
        }
    }
    return ip;
}

}