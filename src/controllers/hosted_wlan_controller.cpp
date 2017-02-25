#include "controllers/hosted_wlan_controller.h"

#include <QCoreApplication>

#include "network/ics_service.h"
#include "network/ics_connection.h"

namespace controllers {

using wlan::HostedWlan;
using network::IcsService;
using network::IcsConnection;

QString HostedWlanController::applyConfigs(
    const std::string &ssid, const std::string &key,
    const std::string &sharing_conn) {
  auto trimmed = QString::fromStdString(ssid).trimmed();
  if (trimmed.isEmpty()) return QCoreApplication::translate(STR(HostedWlanController), "SSID can't be blank");

  if (key.length() < 8 || key.length() > 63)
    return QCoreApplication::translate(STR(HostedWlanController), "Length of key should be between 8 and 63");

  auto ret = this->wlan_->setEnabled(true);
  if (ret != wlan_hosted_network_reason_success) {
    return QCoreApplication::translate(STR(HostedWlan), HostedWlan::getFailReason(ret).c_str());
  }

  ret = this->wlan_->setProperties(ssid.c_str(), key.c_str());
  if (ret != wlan_hosted_network_reason_success) {
    return QCoreApplication::translate(STR(HostedWlan), HostedWlan::getFailReason(ret).c_str());
  }

  if (sharing_conn.empty()) {
    IcsConnection conn;
    if (IcsService::instance()->findById(this->wlan_->adapterID(), conn))
      conn.config()->DisableSharing();
  } else {
    IcsService::instance()->enableSharing(sharing_conn,
                                          this->wlan_->adapterID());
  }

  return "";
}
}
