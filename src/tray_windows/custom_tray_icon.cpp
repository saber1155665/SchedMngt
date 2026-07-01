#include "custom_tray_icon.h"
#include "sched_mngt_utils.h"

custom_tray_icon::custom_tray_icon(QObject* parent) :
    QSystemTrayIcon(parent)
{
    setIcon(QIcon(QPixmap(":/resource/icon/tray_windows.ico")));//设定托盘图标，引号内是自定义的png图片路径
    setToolTip(QStringLiteral("日程管理"));

    //弹出菜单
    m_pTrayMenuWidget = new tray_menu_widget;

    connect(this, &QSystemTrayIcon::activated, this, &custom_tray_icon::OnSystemTrayClicked);
}

int custom_tray_icon::OnSystemTrayClicked(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Context) {
        QPoint point = getGlobalMousePos();
        point.setX(point.x() - m_pTrayMenuWidget->width());
        point.setY(point.y() - m_pTrayMenuWidget->height());
        m_pTrayMenuWidget->move(point);
        m_pTrayMenuWidget->show();
        return 0 ;
    } else if (reason == QSystemTrayIcon::DoubleClick) {
        emit CustomTrayMsg(TM_DoubleClick);
    }
    return 0;
}
