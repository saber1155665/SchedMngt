#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "run_timed_task.h"
#include "main_update_bottom.h"
#include "custom_tray_icon.h"
#include "fast_start_key_value.h"
#include "develop_toolbox_main.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 重写事件处理函数
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    //槽， 快捷输入启动
     void OnFastStartDevelopToolbox(const TreeItemAction action);
private:
    //计算指定页的月份日期表
    void CalcPageDate(int nPage = 0);
    //退出窗口
    void Exit();
    //窗口最小化
    void MinSizeWindow();
    //双击/单击托盘事件
    int OnCustomTrayClicked(custom_tray_icon::TrayMsg msg);

    //显示添加定时任务窗口
    void ShowTimedTaskMgmt();
    //显示工作日志
    void ShowWorkLogMgmt();
    //显示日志信息
    void PromptShowInfo();
    //显示开发工具箱
    void DevelopToolbox();

private:
    Ui::MainWindow *ui;
    //
    run_timed_task *m_pRunTimedTask;
    // 托盘
    custom_tray_icon *m_customTray;//托盘图标添加成员
    //定时器检测鼠标位置
    QTimer m_timerCheckMousePos;
    //
    std::shared_ptr<develop_toolbox_main> m_ptrDevelopToolbox;
//////////////////////////////////////////////////
    // 拖动相关变量
    bool m_isDragging;          // 是否正在拖动窗口
    QPoint m_dragStartPos;      // 鼠标按下时的窗口位置
    QPoint m_mouseStartPos;     // 鼠标按下时的屏幕位置
    // 缩放相关变量
    enum ResizeDirection {
        None, Left, Right, Bottom
    };
    ResizeDirection m_resizeDir; // 当前缩放方向
    bool m_isResizing;           // 是否正在缩放窗口
    QRect m_windowStartRect;    // 缩放开始时的窗口矩形
    QPoint m_resizeStartPos;    // 缩放开始时的鼠标位置
/////////////////////////////////////////////////////////////
};
#endif // MAINWINDOW_H
