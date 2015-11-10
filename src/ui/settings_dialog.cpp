#include "ui/settings_dialog.h"

#include <QCloseEvent>

#include "app_settings.h"

namespace ui {

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
  this->ui_.setupUi(this);
  this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void SettingsDialog::reload() {
  AppSettings::instance()->read();
  this->ui_.chkRunOnStartup->setChecked(
      AppSettings::instance()->run_on_startup);
  this->ui_.chkTurnOnInitially->setChecked(
      AppSettings::instance()->turn_on_initially);
  this->ui_.chkHideWindowInitially->setChecked(
      AppSettings::instance()->hide_window_initially);
}

void SettingsDialog::closeEvent(QCloseEvent *event) {
  AppSettings::instance()->run_on_startup =
      this->ui_.chkRunOnStartup->isChecked();
  AppSettings::instance()->turn_on_initially =
      this->ui_.chkTurnOnInitially->isChecked();
  AppSettings::instance()->hide_window_initially =
      this->ui_.chkHideWindowInitially->isChecked();
  AppSettings::instance()->write();

  event->accept();
}
}
