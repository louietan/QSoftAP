#pragma once

#include <memory>
#include <map>

#include <QObject>

#include <windows.h>
#include <Wlanapi.h>

#include "commons.h"
#include "wlan/wlan_host.h"

namespace wlan {

/**
 * 托管WLAN，对native api的封装
 */
class HostedWlan : public QObject,
                   public std::enable_shared_from_this<HostedWlan> {
  Q_OBJECT

  NON_COPYABLE(HostedWlan)
 public:
  static QString getErrorMsg(DWORD code);
  static std::string getFailReason(WLAN_HOSTED_NETWORK_REASON code);

  HostedWlan();
  ~HostedWlan();

  /**
   * 设置属性
   */
  WLAN_HOSTED_NETWORK_REASON setProperties(const std::string &ssid,
                                           const std::string &key,
                                           DWORD dwMaxNumberOfPeers = 100);

  /**
   * 设置是否启用
   *
   * @param enable,TURE 或 FALSE
   */
  WLAN_HOSTED_NETWORK_REASON setEnabled(BOOL enable);

  //打开
  WLAN_HOSTED_NETWORK_REASON turnOn();

  //关闭
  WLAN_HOSTED_NETWORK_REASON turnOff();

  //在“开”、“关”间切换
  WLAN_HOSTED_NETWORK_REASON toggle();

  /**
   * 重新读取设置信息。包括ssid，密码等.
   *
   * @return 返回错误码，成功为ERROR_SUCCESS。可通过getErrorMsg获取文字描述
   */
  DWORD updateProperties();

  /**
   * 更新当前状态.
   *
   * @return 返回错误码，成功为ERROR_SUCCESS。可通过getErrorMsg获取文字描述
   */
  DWORD updateStatus();

  std::string ssid() const { return this->ssid_; }

  //获取密钥
  std::string secondaryKey() const { return this->secondary_key_; }

  std::string adapterID() const { return this->adapter_id_; }

  WLAN_HOSTED_NETWORK_STATE state() const { return this->state_; }

  /**
   * 虚拟wlan中的主机集合，键是mac
   */
  std::map<std::string, std::shared_ptr<WlanHost>> peers;

 signals:
  //有主机加入
  void peerJoined(std::shared_ptr<HostedWlan> sender,
                  std::shared_ptr<WlanHost> peer);
  //有主机断开
  void peerLeaved(std::shared_ptr<HostedWlan> sender, std::string mac);
  //虚拟wifi状态发生改变
  void stateChanged(std::shared_ptr<HostedWlan> sender);

 private:
  static void wlanCallback(PWLAN_NOTIFICATION_DATA data, PVOID ctx);

  void init();
  void release();

  HANDLE client_handle_ = nullptr;
  std::string ssid_;
  std::string secondary_key_;
  DWORD dw_number_of_max_peers_    = 100;
  WLAN_HOSTED_NETWORK_STATE state_ = {};
  std::string adapter_id_;
  GUID adapter_id_raw_           = {};
  DOT11_MAC_ADDRESS mac_address_ = {};
};
}
