#include "ui/shared_folders_table_model.h"

namespace ui {

SharedFoldersTableModel::SharedFoldersTableModel(QObject *parent)
    : QAbstractTableModel(parent) {}

void SharedFoldersTableModel::setShares(
    const std::vector<sharing::SharedResource> &shares) {
  this->shares_.clear();
  for (auto i : shares) {
    if (i.type == STYPE_DISKTREE) this->shares_.push_back(i);
  }
}

int SharedFoldersTableModel::rowCount(const QModelIndex &parent) const {
  return this->shares_.size();
}

QVariant SharedFoldersTableModel::data(const QModelIndex &index,
                                       int role) const {
  if (!index.isValid() || role != Qt::DisplayRole) return QVariant();

  if (index.row() >= this->shares_.size() || index.row() < 0) return QVariant();

  auto item = this->shares_[index.row()];
  switch (index.column()) {
    case 0:
      return QString::fromStdWString(item.name);
    case 1:
      return QString::fromStdWString(item.path);
    default:
      return QVariant();
  }
}
QVariant SharedFoldersTableModel::headerData(int section,
                                             Qt::Orientation orientation,
                                             int role) const {
  if (role != Qt::DisplayRole) return QVariant();

  if (orientation == Qt::Orientation::Horizontal) {
    switch (section) {
      case 0:
        return "name";
      case 1:
        return "folder path";
      default:
        return QVariant();
    }
  } else {
    return section + 1;
  }
}
}
