#pragma once

#include <memory>

#include <QtWidgets/QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>

#include "ui_mainwindow.h"
#include "ui/settings_dialog.h"
#include "ui/wlan_host_table_model.h"
#include "ui/shared_folders_table_model.h"
#include "wlan/hosted_wlan.h"
#include "controllers/hosted_wlan_controller.h"

namespace ui {
class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(std::shared_ptr<wlan::HostedWlan> wlan, QWidget *parent = nullptr);
  protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
  private slots:
    /// @{界面事件处理函数
    void on_btnSwitch_clicked();
    void on_btnApplySettings_clicked();
    void on_tblvwShares_customContextMenuRequested(const QPoint &loc);
    void on_tblvwHosts_customContextMenuRequested(const QPoint &loc);
    void on_btnReloadConnections_clicked();
    void on_sharing_action();
    /// @}
    void reloadPeers();
  private:
    /// @{初始化函数
    void setupExtraUi();
    void connectViewSignals();
    void keepViewSync();
    /// @}

    void updateWlanView();
    void updatePeersView();
    void updateSharingView();

    void log(const QString &msg);

    std::shared_ptr<wlan::HostedWlan> wlan_;
    controllers::HostedWlanController wlan_controller_;

    /// @ui{相关
    QIcon ico_refresh_;
    QIcon ico_wifi_on_;
    QIcon ico_wifi_off_;
    QMenu ctx_menu_peers_;
    QMenu ctx_menu_sharing_;
    QMenu ctx_menu_tray_;
    QSystemTrayIcon tray_;
    SettingsDialog dlg_settings_;
    Ui::MainWindowClass ui_;
    WlanHostTableModel host_table_model_;
    SharedFoldersTableModel shared_folders_model_;
    /// @}
};
}
