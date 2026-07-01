#ifndef CUSTOM_TRAY_ICON_H
#define CUSTOM_TRAY_ICON_H
#include "tray_menu_widget.h"

#include <QSystemTrayIcon>

class custom_tray_icon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    enum TrayMsg {
        TM_DoubleClick,
        TM_XXX,

        TM_Count,
    };
    custom_tray_icon(QObject* parent = nullptr);


Q_SIGNALS:
    void CustomTrayMsg(TrayMsg msg);

private:
    //消息
    int OnSystemTrayClicked(QSystemTrayIcon::ActivationReason reason);
private:
    //弹出菜单
    tray_menu_widget *m_pTrayMenuWidget;
};

#endif // CUSTOM_TRAY_ICON_H
