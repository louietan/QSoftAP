#include "wlan/hosted_wlan.h"

#include <array>

#include "wlan/wlan_host.h"
#include "network/net_utils.h"

namespace wlan {

using wlan::WlanHost;
using network::NetUtils;

HostedWlan::HostedWlan() { this->init(); }

HostedWlan::~HostedWlan() { this->release(); }

void HostedWlan::wlanCallback(PWLAN_NOTIFICATION_DATA data, PVOID ctx) {
  auto self = static_cast<HostedWlan *>(ctx);
  switch (data->NotificationCode) {
    case wlan_hosted_network_state_change: {
      auto info    = static_cast<PWLAN_HOSTED_NETWORK_STATE_CHANGE>(data->pData);
      self->state_ = info->NewState;
      if (self->state_ != wlan_hosted_network_active) self->peers.clear();
      emit self->stateChanged(self->shared_from_this());
    } break;
    case wlan_hosted_network_peer_state_change: {
      auto info =
          static_cast<PWLAN_HOSTED_NETWORK_DATA_PEER_STATE_CHANGE>(data->pData);
      switch (info->PeerStateChangeReason) {
        case wlan_hosted_network_reason_peer_arrived: {
          auto peer                          = std::make_shared<WlanHost>(info->NewState);
          self->peers[peer->mac_address_hex] = peer;
          emit self->peerJoined(self->shared_from_this(), peer);
        } break;
        case wlan_hosted_network_reason_peer_departed: {
          auto mac =
              NetUtils::readableMacAddress(info->NewState.PeerMacAddress);
          auto itr = self->peers.find(mac);
          if (itr != self->peers.end()) self->peers.erase(itr);
          emit self->peerLeaved(self->shared_from_this(), mac);
        } break;
        default:
          break;
      }
    } break;
  }
}

void HostedWlan::init() {
  DWORD pdwNegotiatedVersion;
  WlanOpenHandle(2, nullptr, &pdwNegotiatedVersion, &this->client_handle_);

  WlanHostedNetworkInitSettings(this->client_handle_, nullptr, nullptr);

  //注册回调
  WlanRegisterNotification(this->client_handle_,
                           WLAN_NOTIFICATION_SOURCE_HNWK,  //只监听托管网络
                           TRUE, WLAN_NOTIFICATION_CALLBACK(&wlanCallback),
                           this, nullptr, nullptr);
}

void HostedWlan::release() {
  if (this->client_handle_) {
    WlanCloseHandle(this->client_handle_, nullptr);
  }
}

WLAN_HOSTED_NETWORK_REASON HostedWlan::setProperties(const std::string &ssid,
                                                     const std::string &key,
                                                     DWORD dwMaxNumberOfPeers) {
  DOT11_SSID dot11_ssid  = {};
  dot11_ssid.uSSIDLength = ssid.length();
  memcpy(dot11_ssid.ucSSID, ssid.data(), ssid.length());
  WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS connection_info = {};
  connection_info.dwMaxNumberOfPeers                      = dwMaxNumberOfPeers;
  connection_info.hostedNetworkSSID                       = dot11_ssid;

  //设置连接信息
  WLAN_HOSTED_NETWORK_REASON reason;
  auto ret = WlanHostedNetworkSetProperty(
      this->client_handle_, wlan_hosted_network_opcode_connection_settings,
      sizeof(connection_info), &connection_info, &reason, nullptr);

  if (ret != ERROR_SUCCESS || reason != wlan_hosted_network_reason_success)
    return reason;

  //设置密钥
  ret = WlanHostedNetworkSetSecondaryKey(
      this->client_handle_,
      key.length() + 1,  //要算上终结符'\0'
      PUCHAR(key.data()),
      TRUE,  //这里表示传入的密钥是明文密码。密钥有两种形式：8到63个字节 或
      // 64个十六进制数字（即32字节）
      TRUE, &reason, nullptr);

  if (ret == ERROR_SUCCESS && reason == wlan_hosted_network_reason_success) {
    this->updateProperties();
  }

  return reason;
}

WLAN_HOSTED_NETWORK_REASON HostedWlan::turnOn() {
  WLAN_HOSTED_NETWORK_REASON reason;
  WlanHostedNetworkForceStart(this->client_handle_, &reason, nullptr);
  WlanRegisterVirtualStationNotification(this->client_handle_, TRUE, nullptr);
  return reason;
}

WLAN_HOSTED_NETWORK_REASON HostedWlan::turnOff() {
  WLAN_HOSTED_NETWORK_REASON reason;
  WlanHostedNetworkForceStop(this->client_handle_, &reason, nullptr);
  return reason;
}

WLAN_HOSTED_NETWORK_REASON HostedWlan::toggle() {
  if (this->state() != wlan_hosted_network_active) return this->turnOn();

  return this->turnOff();
}

WLAN_HOSTED_NETWORK_REASON HostedWlan::setEnabled(BOOL enable) {
  WLAN_HOSTED_NETWORK_REASON reason;
  WlanHostedNetworkSetProperty(this->client_handle_,
                               wlan_hosted_network_opcode_enable, sizeof(BOOL),
                               &enable, &reason, nullptr);

  return reason;
}

DWORD HostedWlan::updateProperties() {
  DWORD key_len;
  PUCHAR pkey = nullptr;
  BOOL bIsPassPhrase;
  BOOL bPersistent;
  //查密钥
  auto ret = WlanHostedNetworkQuerySecondaryKey(this->client_handle_, &key_len,
                                                &pkey, &bIsPassPhrase,
                                                &bPersistent, nullptr, nullptr);

  if (ret != ERROR_SUCCESS) return ret;

  if (pkey == nullptr)
    this->secondary_key_ = '\0';
  else
    this->secondary_key_ = reinterpret_cast<char *>(pkey);

  WlanFreeMemory(pkey);

  DWORD data_size;
  PVOID data = nullptr;
  WLAN_OPCODE_VALUE_TYPE wlanOpcodeValueType;
  //查连接信息
  ret = WlanHostedNetworkQueryProperty(
      this->client_handle_, wlan_hosted_network_opcode_connection_settings,
      &data_size, &data, &wlanOpcodeValueType, nullptr);

  if (ret != ERROR_SUCCESS) return ret;

  auto settings                 = static_cast<WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS *>(data);
  this->ssid_                   = reinterpret_cast<char *>(settings->hostedNetworkSSID.ucSSID);
  this->dw_number_of_max_peers_ = settings->dwMaxNumberOfPeers;

  return ret;
}

DWORD HostedWlan::updateStatus() {
  PWLAN_HOSTED_NETWORK_STATUS status = nullptr;
  auto ret =
      WlanHostedNetworkQueryStatus(this->client_handle_, &status, nullptr);

  if (ret == ERROR_SUCCESS) {
    this->peers.clear();
    for (auto i = 0; i < status->dwNumberOfPeers; ++i) {
      auto peer                          = std::make_shared<WlanHost>(status->PeerList[i]);
      this->peers[peer->mac_address_hex] = peer;
    }

    this->state_ = status->HostedNetworkState;

    this->adapter_id_raw_ = status->IPDeviceID;
    std::array<char, 100> adapter_id;
    sprintf(
        adapter_id.data(),
        "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
        status->IPDeviceID.Data1, status->IPDeviceID.Data2,
        status->IPDeviceID.Data3, status->IPDeviceID.Data4[0],
        status->IPDeviceID.Data4[1], status->IPDeviceID.Data4[2],
        status->IPDeviceID.Data4[3], status->IPDeviceID.Data4[4],
        status->IPDeviceID.Data4[5], status->IPDeviceID.Data4[6],
        status->IPDeviceID.Data4[7]);
    this->adapter_id_ = adapter_id.data();

    sprintf(
        adapter_id.data(),
        "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        status->IPDeviceID.Data1, status->IPDeviceID.Data2,
        status->IPDeviceID.Data3, status->IPDeviceID.Data4[0],
        status->IPDeviceID.Data4[1], status->IPDeviceID.Data4[2],
        status->IPDeviceID.Data4[3], status->IPDeviceID.Data4[4],
        status->IPDeviceID.Data4[5], status->IPDeviceID.Data4[6],
        status->IPDeviceID.Data4[7]);

    memcpy(this->mac_address_, status->wlanHostedNetworkBSSID,
           sizeof(this->mac_address_));

    WlanFreeMemory(status);  //别忘了释放内存
  }
  return ret;
}

QString HostedWlan::getErrorMsg(DWORD code) {
  std::array<wchar_t, 500> buffer;
  WlanReasonCodeToString(code, buffer.size(), buffer.data(), nullptr);
  return QString::fromWCharArray(buffer.data());
}

std::string HostedWlan::getFailReason(WLAN_HOSTED_NETWORK_REASON code) {
  switch (code) {
    case wlan_hosted_network_reason_unspecified:
      return "unspecified";
    case wlan_hosted_network_reason_bad_parameters:
      return "bad_parameters";
    case wlan_hosted_network_reason_service_shutting_down:
      return "service_shutting_down";
    case wlan_hosted_network_reason_insufficient_resources:
      return "wlan_hosted_network_reason_insufficient_resources";
    case wlan_hosted_network_reason_elevation_required:
      return "elevation_required";
    case wlan_hosted_network_reason_read_only:
      return "read_only";
    case wlan_hosted_network_reason_persistence_failed:
      return "persistence_failed";
    case wlan_hosted_network_reason_crypt_error:
      return "crypt_error";
    case wlan_hosted_network_reason_impersonation:
      return "impersonation";
    case wlan_hosted_network_reason_stop_before_start:
      return "stop_before_start";
    case wlan_hosted_network_reason_interface_available:
      return "interface_available";
    case wlan_hosted_network_reason_interface_unavailable:
      return "interface_unavailable";
    case wlan_hosted_network_reason_miniport_stopped:
      return "miniport_stopped";
    case wlan_hosted_network_reason_miniport_started:
      return "miniport_started";
    case wlan_hosted_network_reason_incompatible_connection_started:
      return "incompatible_connection_started";
    case wlan_hosted_network_reason_incompatible_connection_stopped:
      return "incompatible_connection_stopped";
    case wlan_hosted_network_reason_user_action:
      return "user_action";
    case wlan_hosted_network_reason_client_abort:
      return "client_abort";
    case wlan_hosted_network_reason_ap_start_failed:
      return "ap_start_failed";
    case wlan_hosted_network_reason_peer_arrived:
      return "peer_arrived";
    case wlan_hosted_network_reason_peer_departed:
      return "peer_departed";
    case wlan_hosted_network_reason_peer_timeout:
      return "peer_timeout";
    case wlan_hosted_network_reason_gp_denied:
      return "gp_denied";
    case wlan_hosted_network_reason_service_unavailable:
      return "service_unavailable";
    case wlan_hosted_network_reason_device_change:
      return "device_change";
    case wlan_hosted_network_reason_properties_change:
      return "properties_change";
    case wlan_hosted_network_reason_virtual_station_blocking_use:
      return "virtual_station_blocking_use";
    case wlan_hosted_network_reason_service_available_on_virtual_station:
      return "service_available_on_virtual_station";
    default:
      return "";
  }
}
}
