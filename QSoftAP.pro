TARGET = QtSoftAP
TEMPLATE = app

QT += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QMAKE_LFLAGS_EXE  += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"

FORMS += \
    forms/add_sharing.ui \
    forms/mainwindow.ui \
    forms/settings.ui

RESOURCES += \
    Resources/mainwindow.qrc \
    Resources/resources.qrc

DISTFILES += \
    Resources/images/refresh.ico \
    Resources/images/switch_off.bmp \
    Resources/images/switch_on.bmp \
    Resources/images/wifi_off.ico \
    Resources/images/wifi_on.ico \
    translations/zh_CN.ts

INCLUDEPATH =./include

HEADERS += \
    Resources/version.rc \
    include/app_settings.h \
    include/commons.h \
    include/controllers/hosted_wlan_controller.h \
    include/network/ics_connection.h \
    include/network/ics_service.h \
    include/network/net_utils.h \
    include/sharing/shared_resource.h \
    include/sharing/sharing_service.h \
    include/ui/add_sharing_dialog.h \
    include/ui/mainwindow.h \
    include/ui/settings_dialog.h \
    include/ui/shared_folders_table_model.h \
    include/ui/wlan_host_table_model.h \
    include/wlan/hosted_wlan.h \
    include/wlan/wlan_host.h

SOURCES += \
    src/app_settings.cpp \
    src/controllers/hosted_wlan_controller.cpp \
    src/main.cpp \
    src/network/ics_connection.cpp \
    src/network/ics_service.cpp \
    src/network/net_utils.cpp \
    src/sharing/sharing_service.cpp \
    src/ui/add_sharing_dialog.cpp \
    src/ui/mainwindow.cpp \
    src/ui/settings_dialog.cpp \
    src/ui/shared_folders_table_model.cpp \
    src/ui/wlan_host_table_model.cpp \
    src/wlan/hosted_wlan.cpp \
    src/wlan/wlan_host.cpp

LIBS+=wlanapi.lib netapi32.lib oleaut32.lib  Ws2_32.lib Iphlpapi.lib ole32.lib

