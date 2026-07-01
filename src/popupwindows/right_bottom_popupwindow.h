#ifndef RIGHT_BOTTOM_POPUPWINDOW_H
#define RIGHT_BOTTOM_POPUPWINDOW_H

#include "sqlite_tools.h"

#include <QDialog>
#include <QList>
#include <QMutex>
#include <QTimer>
#include <QPropertyAnimation>

namespace Ui {
class right_bottom_popupwindow;
}

class right_bottom_popupwindow : public QDialog
{
    Q_OBJECT

public:
    explicit right_bottom_popupwindow(QWidget *parent = nullptr);
    ~right_bottom_popupwindow();

    //显示
    void show_popup();
    //增加任务
    void PushTask(const TimedTaskItem &item);
    //获取任务
    bool PopTask(TimedTaskItem &item);
private:
    //检查一次是否有任务要显示，如果有就显示
    void TimerCheckTimedTask();
    //动画效果
    void startAnimation();
    //启动隐藏
    void AutoHideWin();
    //用户自己点击了确认
    void OkHide();
private:
    Ui::right_bottom_popupwindow *ui;
    //锁
    QMutex m_mutex;
    //队列
    QList<TimedTaskItem> m_listTask;
    //定时器，获取要提示的信息
    QTimer m_timer;
    //右下角冒出动画
    QPropertyAnimation *m_pAnimation;
    //定时器，到时间之后自动隐藏
    QTimer m_timerAutoHide;
    //隐藏时间
    int m_nHideTime;
};

#endif // RIGHT_BOTTOM_POPUPWINDOW_H
