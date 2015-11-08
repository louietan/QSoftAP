#include <Winsock2.h>

#include "network/net_utils.h"
#include "wlan/wlan_host.h"

namespace wlan {

using network::NetUtils;

WlanHost::WlanHost(const WLAN_HOSTED_NETWORK_PEER_STATE &peer) : peer_state_(peer)
{
    this->mac_address_hex = NetUtils::readableMacAddress(PUCHAR(peer.PeerMacAddress));
}

}
