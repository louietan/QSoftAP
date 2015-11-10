#pragma once

#include <QDialog>

#include "ui_add_sharing.h"

namespace ui {

class AddSharingDialog : public QDialog {
 public:
  AddSharingDialog(QWidget *parent = nullptr);

 private:
  Ui::AddSharingDialog ui_;
};
}