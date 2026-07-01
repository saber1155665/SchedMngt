#ifndef TIMED_TASK_H
#define TIMED_TASK_H

#include "sqlite_tools.h"

#include <QWidget>
#include <QDialog>
#include <QMap>

namespace Ui {
class timed_task;
}

class timed_task : public QDialog
{
    Q_OBJECT

public:
    explicit timed_task(QWidget *parent = nullptr);
    ~timed_task();

    //初始化运行类型和运行显示名的关系
    static QMap<QString ,TimedTaskRunType> InitRunTypeAndString();

signals:
    //信号，发送给父窗口。添加了新的定时任务
    void SendAddNewTimedTask();

private:
    //退出窗口
    void Exit();
    //添加项
    void Add();

    //添加运行方式
    void AddCombBoxRunType();
    //下拉日历
    void DropDownCalender();
private slots:
    //信号，获取选择时间的窗口的结果
    void TimedTaskSelect(QString qstrDateTime);
private:
    Ui::timed_task *ui;
    //
    QMap<QString ,TimedTaskRunType> m_qmapRunType;
};

#endif // TIMED_TASK_H
