#pragma once

#include <QAbstractTableModel>

#include "sharing/shared_resource.h"

namespace ui {

class SharedFoldersTableModel : public QAbstractTableModel {
  Q_OBJECT
 public:
  SharedFoldersTableModel(QObject *parent = nullptr);
  void setShares(const std::vector<sharing::SharedResource> &shares);

  virtual int rowCount(
      const QModelIndex &parent = QModelIndex()) const override;
  virtual int columnCount(
      const QModelIndex &parent = QModelIndex()) const override {
    return 2;
  }
  virtual QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const override;

 private:
  std::vector<sharing::SharedResource> shares_;
};
}