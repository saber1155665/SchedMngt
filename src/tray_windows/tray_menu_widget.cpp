#include "tray_menu_widget.h"
#include "ui_tray_menu_widget.h"
#include "sqlite_tools.h"

tray_menu_widget::tray_menu_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tray_menu_widget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::Window | Qt::WindowStaysOnTopHint);

    //关联
    connect(ui->btn_exit, &QPushButton::clicked, this, &tray_menu_widget::Exit);
}

tray_menu_widget::~tray_menu_widget()
{
    delete ui;
}

//退出进程
void tray_menu_widget::Exit() {
    //
    sqlite_tools::get_sqlite_tools().CloseDb();
    //退出
    QApplication::quit();
}


//鼠标离开事件
void tray_menu_widget::leaveEvent(QEvent *event)  {
    hide();
}
