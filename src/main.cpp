#include <memory>
#include <array>

#include <QtWidgets/QApplication>

#include "network/ics_service.h"
#include "ui/mainwindow.h"
#include "wlan/hosted_wlan.h"
#include "app_settings.h"

using wlan::HostedWlan;
using wlan::WlanHost;
using ui::MainWindow;
using network::IcsService;

Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(std::shared_ptr<HostedWlan>);
Q_DECLARE_METATYPE(std::shared_ptr<WlanHost>);
Q_DECLARE_METATYPE(WLAN_HOSTED_NETWORK_REASON);
Q_DECLARE_METATYPE(QVector<int>);

void registerMetaTyeps() {
  qRegisterMetaType<std::string>();
  qRegisterMetaType<std::shared_ptr<HostedWlan>>();
  qRegisterMetaType<std::shared_ptr<WlanHost>>();
  qRegisterMetaType<WLAN_HOSTED_NETWORK_REASON>();
  qRegisterMetaType<QVector<int>>();
}

int main(int argc, char *argv[]) {
  registerMetaTyeps();

  QApplication a(argc, argv);
  a.setQuitOnLastWindowClosed(false);

  auto wlan = std::make_shared<HostedWlan>();
  if (AppSettings::instance()->turn_on_initially) wlan->turnOn();

  MainWindow w(wlan);
  if (!AppSettings::instance()->hide_window_initially) w.show();

  return a.exec();
}
