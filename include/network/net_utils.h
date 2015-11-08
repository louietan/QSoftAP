#pragma once

#include <string>

#include <windows.h>

namespace network
{
/**
 * 提供网络相关的工具函数
 */
class NetUtils
{
  public:
    /**
     * 把mac地址转成16进制字符串形式
     */
    static std::string readableMacAddress(unsigned char bytes[], size_t len = 6, char delim = '-');

    /**
     * 根据mac取ip
     * @param[in] mac 6个字节,不是十六进制字符串
     */
    static DWORD findIpByMac(unsigned char mac[6]);
};
}
