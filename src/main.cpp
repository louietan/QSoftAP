#include <array>
#include <memory>

#include <QApplication>
#include <QTranslator>

#include "app_settings.h"
#include "network/ics_service.h"
#include "ui/mainwindow.h"
#include "wlan/hosted_wlan.h"

using network::IcsService;
using ui::MainWindow;
using wlan::HostedWlan;
using wlan::WlanHost;

Q_DECLARE_METATYPE(QVector<int>);
Q_DECLARE_METATYPE(WLAN_HOSTED_NETWORK_REASON);
Q_DECLARE_METATYPE(std::shared_ptr<HostedWlan>);
Q_DECLARE_METATYPE(std::shared_ptr<WlanHost>);
Q_DECLARE_METATYPE(std::string);

void registerMetaTyeps() {
  qRegisterMetaType<QVector<int>>();
  qRegisterMetaType<WLAN_HOSTED_NETWORK_REASON>();
  qRegisterMetaType<std::shared_ptr<HostedWlan>>();
  qRegisterMetaType<std::shared_ptr<WlanHost>>();
  qRegisterMetaType<std::string>();
}

int main(int argc, char *argv[]) {
  registerMetaTyeps();

  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);

  QTranslator appTranslator;
  appTranslator.load(QLocale::system().name() + ".qm",
                     qApp->applicationDirPath() + "/languages");
  app.installTranslator(&appTranslator);

  auto wlan = std::make_shared<HostedWlan>();
  if (AppSettings::instance()->turn_on_initially) wlan->turnOn();

  MainWindow w(wlan);
  if (!AppSettings::instance()->hide_window_initially) w.show();

  return app.exec();
}
