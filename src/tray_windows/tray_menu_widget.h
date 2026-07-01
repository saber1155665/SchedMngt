#ifndef TRAY_MENU_WIDGET_H
#define TRAY_MENU_WIDGET_H

#include <QWidget>

namespace Ui {
class tray_menu_widget;
}

class tray_menu_widget : public QWidget
{
    Q_OBJECT

public:
    explicit tray_menu_widget(QWidget *parent = nullptr);
    ~tray_menu_widget();

private:
    //退出进程
    void Exit();
    //鼠标离开事件
    void leaveEvent(QEvent *event) override;
private:
    Ui::tray_menu_widget *ui;
};

#endif // TRAY_MENU_WIDGET_H
