#pragma once

#include <string>
#include <memory>

#include "wlan/hosted_wlan.h"

namespace controllers {

class HostedWlanController {
 public:
  HostedWlanController(std::shared_ptr<wlan::HostedWlan> hosted_wlan)
      : wlan_(hosted_wlan) {}

  /**
   * 应用配置
   * 
   * @param[in] ssid
   * @param[in] key
   * @param[in] sharing_conn 共享连接的id，如果为空则取消共享
   * @return 错误消息，空串表示成功
   */
  QString applyConfigs(const std::string &ssid, const std::string &key,
                            const std::string &sharing_conn);

 private:
  std::shared_ptr<wlan::HostedWlan> wlan_;
};
}