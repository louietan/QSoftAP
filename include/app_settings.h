#pragma once

#include <QSettings>

#include "commons.h"

class AppSettings {
  SINGLETON(AppSettings)
 public:
  static const char *const APP_NAME;

  void read();
  void write();
  bool run_on_startup;
  bool turn_on_initially;
  bool hide_window_initially;

 protected:
  AppSettings();

 private:
  QSettings auto_start_;
  QSettings others_;
};