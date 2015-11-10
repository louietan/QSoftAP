#include "app_settings.h"

#include <QCoreApplication>

const char *const AppSettings::APP_NAME = "QSoftAP";

AppSettings::AppSettings()
    : auto_start_(
          "HKEY_LOCAL_"
          "MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
          QSettings::NativeFormat),
      others_("configs.ini", QSettings::IniFormat) {
  this->read();
}

void AppSettings::read() {
  auto command            = auto_start_.value(APP_NAME).toString();
  this->run_on_startup    = !command.isEmpty();
  this->turn_on_initially = others_.value("turn_on_initially").value<bool>();
  this->hide_window_initially =
      others_.value("hide_window_initially").value<bool>();
}
void AppSettings::write() {
  if (this->run_on_startup) {
    auto value = QCoreApplication::applicationFilePath();
    value.replace("/", "\\");
    auto_start_.setValue(APP_NAME, value);
  } else {
    auto_start_.remove(APP_NAME);
  }
  auto_start_.sync();

  others_.setValue("turn_on_initially", this->turn_on_initially);
  others_.setValue("hide_window_initially", this->hide_window_initially);
  others_.sync();
}