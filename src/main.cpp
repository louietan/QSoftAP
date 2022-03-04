#include <array>
#include <memory>

#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QMessageBox>
#include <QObject>

#include "app_settings.h"
#include "network/ics_service.h"
#include "ui/mainwindow.h"
#include "wlan/hosted_wlan.h"

using network::IcsService;
using ui::MainWindow;
using wlan::HostedWlan;
using wlan::WlanHost;

Q_DECLARE_METATYPE (QVector<int>);
Q_DECLARE_METATYPE (WLAN_HOSTED_NETWORK_REASON);
Q_DECLARE_METATYPE (std::shared_ptr<HostedWlan>);
Q_DECLARE_METATYPE (std::shared_ptr<WlanHost>);
Q_DECLARE_METATYPE (std::string);

void registerMetaTyeps()
{
	qRegisterMetaType<QVector<int>>();
	qRegisterMetaType<WLAN_HOSTED_NETWORK_REASON>();
	qRegisterMetaType<std::shared_ptr<HostedWlan>>();
	qRegisterMetaType<std::shared_ptr<WlanHost>>();
	qRegisterMetaType<std::string>();
}

BOOL IsRunasAdmin()
{
	BOOL bElevated = FALSE;
	HANDLE hToken = nullptr;

	// Get current process token
	if (!OpenProcessToken (GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{ return FALSE; }

	TOKEN_ELEVATION tokenEle;
	DWORD dwRetLen = 0;

	// Retrieve token elevation information
	if (GetTokenInformation (hToken, TokenElevation, &tokenEle, sizeof (tokenEle), &dwRetLen)) {
		if (dwRetLen == sizeof (tokenEle)) {
			bElevated = tokenEle.TokenIsElevated;
		}
	}

	CloseHandle (hToken);
	return bElevated;
}

int main (int argc, char* argv[])
{
	registerMetaTyeps();

	QApplication app (argc, argv);
	app.setQuitOnLastWindowClosed (false);

	QTranslator appTranslator;
	qDebug() << QLocale::system().name() + ".qm" << Qt::endl;
	qDebug() << qApp->applicationDirPath() << Qt::endl;
	appTranslator.load (QLocale::system().name() + ".qm",
						qApp->applicationDirPath() + "/languages");
	app.installTranslator (&appTranslator);


	auto wlan = std::make_shared<HostedWlan>();

	if (AppSettings::instance()->turn_on_initially) { wlan->turnOn(); }

	MainWindow w (wlan);

	if (!AppSettings::instance()->hide_window_initially) {
		w.show();


		if (IsRunasAdmin() != TRUE) {

			QMessageBox msgBox;
			msgBox.setText (QCoreApplication::translate (
								STR (QSoftAP), "Warning."));
			msgBox.setInformativeText (QCoreApplication::translate (
										   STR (QSoftAP), "You must run app with administrator privileges!"));
			msgBox.setStandardButtons (QMessageBox::Ok);
			msgBox.setDefaultButton (QMessageBox::Ok);
			int ret = msgBox.exec();


			return ret;
		}
	}

	return app.exec();
}
