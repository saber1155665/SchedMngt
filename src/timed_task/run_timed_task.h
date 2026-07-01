#ifndef RUN_TIMED_TASK_H
#define RUN_TIMED_TASK_H

#include "sqlite_tools.h"
#include "right_bottom_popupwindow.h"

#include <QObject>
#include <QList>
#include <QMutex>
#include <QThread>
#include <QDateTime>

typedef void (*PFuncCallbackSS)(const TimedTaskItem &item, void *pArgs) ;
class run_timed_task;

class thread_run_task : public QThread {
    Q_OBJECT

public:
    thread_run_task(run_timed_task *p, QObject *parent = nullptr);

signals:
   //槽， 发现需要新处理的定时任务
    void OnPushTimedTaskSignal(const TimedTaskItem &item);
public:
    //线程函数
    void run() override;
    //发送信号
    void SendSignal(const TimedTaskItem &item);
    static void CallbackSS(const TimedTaskItem &item, void *pArgs);

private:
    run_timed_task *m_pRunTimedTask;
};

class run_timed_task : public QObject
{
    Q_OBJECT
public:
    explicit run_timed_task(QObject *parent = nullptr);
    virtual ~run_timed_task();
    //刷新任务列表,从数据库重新查询
   void RefreshTaskList();
   //是否退出
   bool GetExit();
   //线程执行主要函数,找出要提醒的消息。并且执行提醒
   void CheckTimedTask(PFuncCallbackSS cb, void *pArgs);
private slots:
   //槽， 发现需要新处理的定时任务
    void OnPushTimedTask(const TimedTaskItem &item);
private:
   //通过锁，添加到列表
   void ChangeTask(const  QList<TimedTaskItem> &listTimedTask);
   //清理所有任务
   void ClearTask();



private:
    //锁
    QMutex m_mutex;
    //队列
    QList<TimedTaskItem> m_listTask;
    //执行线程
    thread_run_task m_thread;
    //退出
    bool m_bExit;
    //显示提示
    right_bottom_popupwindow *m_pRBPopupWin;
};




#endif // RUN_TIMED_TASK_H
