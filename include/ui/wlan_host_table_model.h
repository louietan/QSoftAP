#pragma once

#include <array>

#include <QAbstractTableModel>

#include "wlan/hosted_wlan.h"
#include "wlan/wlan_host.h"

namespace ui {

/**
 * 主机列表model
 */
class WlanHostTableModel : public QAbstractTableModel {
  Q_OBJECT

 public:
  WlanHostTableModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    return 2;
  }
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole) override;

  bool insertRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;

  std::vector<std::tuple<std::string, std::string, std::string>>
      peers;  //{原始mac，格化后的mac，ip地址}

 public slots:
  void onPeerJoined(std::shared_ptr<wlan::HostedWlan> hostd_wlan,
                    std::shared_ptr<wlan::WlanHost> peer);
  void onPeerLeaved(std::shared_ptr<wlan::HostedWlan> hostd_wlan,
                    std::string mac);
};
}
