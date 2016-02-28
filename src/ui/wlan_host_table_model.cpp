#include "ui/wlan_host_table_model.h"

#include <future>
#include <sstream>

#include <QCoreApplication>

#include "network/net_utils.h"

namespace ui {

using wlan::HostedWlan;
using wlan::WlanHost;
using network::NetUtils;

WlanHostTableModel::WlanHostTableModel(QObject *parent)
    : QAbstractTableModel(parent) {}

int WlanHostTableModel::rowCount(const QModelIndex &parent) const {
  return this->peers.size();
}

QVariant WlanHostTableModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || role != Qt::DisplayRole) return QVariant();

  if (index.row() >= this->peers.size() || index.row() < 0) return QVariant();

  auto itr = this->peers.begin();
  std::advance(itr, index.row());
  switch (index.column()) {
    case 0: {
      return QString::fromStdString(std::get<2>(*itr));
    }
    default:
      return QVariant();
  }
}
QVariant WlanHostTableModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const {
  if (role != Qt::DisplayRole) return QVariant();

  if (orientation == Qt::Orientation::Horizontal) {
    switch (section) {
      case 0:
        return "ip";
      default:
        return QVariant();
    }
  } else {
    return section + 1;
  }
}

bool WlanHostTableModel::insertRows(int row, int count,
                                    const QModelIndex &parent) {
  this->beginInsertRows(QModelIndex(), row, count + row - 1);
  for (auto i = 0; i < count; ++i) {
    this->peers.push_back(std::make_tuple("", "", ""));
  }
  this->endInsertRows();
  return true;
}

bool WlanHostTableModel::removeRows(int row, int count,
                                    const QModelIndex &parent) {
  if (row >= this->peers.size() || row + count - 1 >= this->peers.size())
    return false;

  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for (auto i = 0; i < count; ++i) {
    this->peers.erase(this->peers.begin() + row);
  }
  endRemoveRows();
  return true;
}

bool WlanHostTableModel::setData(const QModelIndex &index,
                                 const QVariant &value, int role) {
  if (index.isValid() && role == Qt::EditRole) {
    auto row = index.row();

    auto &item = this->peers[row];

    if (index.column() == 0) {
      std::get<0>(item) = value.value<QByteArray>().data();
      std::get<1>(item) =
          NetUtils::readableMacAddress(PUCHAR(std::get<0>(item).data()));
    } else if (index.column() == 1) {
      std::get<2>(item) = value.toString().toStdString();
    } else
      return false;

    emit dataChanged(index, index);

    return true;
  }

  return true;
}

void WlanHostTableModel::onPeerJoined(std::shared_ptr<HostedWlan>,
                                      std::shared_ptr<WlanHost> peer) {
  auto row = this->peers.size();
  this->insertRows(row, 1);
  auto idx = this->index(row, 0);
  auto mac_addr =
      QByteArray(reinterpret_cast<char *>(peer->peerState().PeerMacAddress));
  this->setData(idx, mac_addr);
  idx            = this->index(row, 1);
  in_addr ip     = {};
  ip.S_un.S_addr = NetUtils::findIpByMac(peer->peerState().PeerMacAddress);
  if (ip.S_un.S_addr == 0) {
    this->setData(idx, QCoreApplication::translate(STR(WlanHostTableModel), "obtaining IP address..."));
    auto fn_update_ip = [this](QByteArray mac) {
      for (auto i = 0; i < 10; ++i) {
        in_addr ipv4     = {};
        ipv4.S_un.S_addr = NetUtils::findIpByMac(PUCHAR(mac.data()));
        if (ipv4.S_un.S_addr != 0) {
          for (auto itr = this->peers.begin(); itr != this->peers.end();
               ++itr) {
            if (memcmp(std::get<0>(*itr).data(), mac.data(), mac.size()) == 0) {
              auto pos = this->index(itr - this->peers.begin(), 1);
              this->setData(pos, QString::fromLocal8Bit(inet_ntoa(ipv4)));
              return;
            }
          }
          break;
        } else {
          std::this_thread::sleep_for(std::chrono::seconds(3));
        }
      }
      for (auto itr = this->peers.begin(); itr != this->peers.end(); ++itr) {
        if (memcmp(std::get<0>(*itr).data(), mac.data(), mac.size()) == 0) {
          auto pos = this->index(itr - this->peers.begin(), 1);
          this->setData(pos, QCoreApplication::translate(STR(WlanHostTableModel), "failed to get the IP address"));
          break;
        }
      }
    };
    std::async(fn_update_ip, mac_addr);
  } else
    this->setData(idx, QString::fromLocal8Bit(inet_ntoa(ip)));
}

void WlanHostTableModel::onPeerLeaved(std::shared_ptr<HostedWlan>,
                                      std::string mac) {
  for (auto i = 0; i < this->peers.size(); ++i) {
    if (std::get<1>(this->peers[i]) == mac) {
      this->removeRows(i, 1, QModelIndex());
      break;
    }
  }
}
}
