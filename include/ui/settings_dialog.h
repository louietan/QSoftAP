#pragma once

#include <QDialog>

#include "ui_settings.h"

namespace ui {

class SettingsDialog : public QDialog {
 public:
  SettingsDialog(QWidget *parent = nullptr);
  void reload();

 protected:
  void closeEvent(QCloseEvent *event) override;

 private:
  Ui::SettingsDialog ui_;
};
}