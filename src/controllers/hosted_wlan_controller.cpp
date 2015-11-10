#include "controllers/hosted_wlan_controller.h"
#include "network/ics_service.h"
#include "network/ics_connection.h"

namespace controllers {

using wlan::HostedWlan;
using network::IcsService;
using network::IcsConnection;

std::wstring HostedWlanController::applyConfigs(
    const std::string &ssid, const std::string &key,
    const std::string &sharing_conn) {
  auto trimmed = QString::fromStdString(ssid).trimmed();
  if (trimmed.isEmpty()) return L"名称不能为空";

  if (key.length() < 8 || key.length() > 63)
    return L"密码长度应为 8 到 63 个字节";

  auto ret = this->wlan_->setEnabled(true);
  if (ret != wlan_hosted_network_reason_success) {
    return HostedWlan::getFailReason(ret);
  }

  ret = this->wlan_->setProperties(ssid.c_str(), key.c_str());
  if (ret != wlan_hosted_network_reason_success) {
    return HostedWlan::getFailReason(ret);
  }

  if (sharing_conn.empty()) {
    IcsConnection conn;
    if (IcsService::instance()->findById(this->wlan_->adapterID(), conn))
      conn.config()->DisableSharing();
  } else {
    IcsService::instance()->enableSharing(sharing_conn,
                                          this->wlan_->adapterID());
  }

  return L"";
}
}