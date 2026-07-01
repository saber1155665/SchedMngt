#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowFlags(Qt::Window | Qt::FramelessWindowHint); // 移除边框和标题栏
    //启动就隐藏主窗口
    //w.show();
    return a.exec();
}
