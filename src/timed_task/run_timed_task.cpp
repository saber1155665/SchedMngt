#include "run_timed_task.h"
#include "sqlite_tools.h"

#include <QDate>
#include <QTime>


thread_run_task::thread_run_task(run_timed_task *p, QObject *parent) :
    m_pRunTimedTask(p)
    , QThread(parent)
{

}

void thread_run_task::run() {
   if (!m_pRunTimedTask) {
       return;
   }
   while (!m_pRunTimedTask->GetExit()) {
       //检查一次任务
       m_pRunTimedTask->CheckTimedTask(CallbackSS, this);
        //释放CPU
        QThread::sleep(5);
   }
}
//发送信号
void thread_run_task::SendSignal(const TimedTaskItem &item) {
    emit OnPushTimedTaskSignal(item);
}
void thread_run_task::CallbackSS(const TimedTaskItem &item, void *pArgs) {
   thread_run_task *p_thread_run_task = static_cast<thread_run_task *>(pArgs);
   p_thread_run_task->SendSignal(item);
}



run_timed_task::run_timed_task(QObject *parent) : QObject(parent)
  , m_bExit(false)
  , m_thread(this, parent)
{
    //创建提示界面
    m_pRBPopupWin = new right_bottom_popupwindow();
    //m_pRBPopupWin->show();

    //关联槽
    bool b = connect(&m_thread, &thread_run_task::OnPushTimedTaskSignal, this, &run_timed_task::OnPushTimedTask, Qt::BlockingQueuedConnection);
    //启动处理线程
    m_thread.start();
}

run_timed_task::~run_timed_task() {
    m_bExit = true;
    //等线程退出
    m_thread.wait();
}

//刷新任务列表,从数据库重新查询
void run_timed_task::RefreshTaskList() {
    #define PAGE_TIMED_TASK_COUNT 10

    //得到总数
    int nPageCount = sqlite_tools::get_sqlite_tools().GetTimedTaskCount();
    nPageCount = (nPageCount + PAGE_TIMED_TASK_COUNT - 1) / PAGE_TIMED_TASK_COUNT;

    //查询
    QList<TimedTaskItem> listTimedTask;
    for (int i = 0; i < nPageCount; i++) {
         QList<TimedTaskItem> listTmpTimedTask;
        //查询数据库
        sqlite_tools::get_sqlite_tools().QueryTimedTask(i * PAGE_TIMED_TASK_COUNT, PAGE_TIMED_TASK_COUNT, listTmpTimedTask);
        //
        listTimedTask.append(listTmpTimedTask);
    }
    //添加
    ChangeTask(listTimedTask);
}


//通过锁，添加到列表
void run_timed_task::ChangeTask(const  QList<TimedTaskItem> &listTimedTask) {
    QMutexLocker locker(&m_mutex);

     QList<TimedTaskItem> listRunTask;
    //处理上一次执行的日期,我们不找最简单的上一次都当成没执行。那样回导致无法精细
    //控制执行时间。比如每小时执行一次，但是我们希望是我们设计的那个分钟和秒才开始执行
    //得到当前日期和时间
    QDateTime dateTimeCur = QDateTime::currentDateTime();
    for (auto iter = listTimedTask.begin(); iter != listTimedTask.end(); iter++) {
        //添加项
        TimedTaskItem item = *(iter);//都一律采用新内容，因为有可能修改了
        //先查找是否已经存在
        for (auto iterMember = m_listTask.begin(); iterMember != m_listTask.end(); iterMember++) {
            if (iter->nId ==  iterMember->nId) {
                //如果存在，显然我们应用原来的上一次执行时间
                item.qstrLastRunTime = iterMember->qstrLastRunTime;
                break;
            }
        }
        //
        listRunTask.push_back(item);
    }
    //替换内容
    m_listTask = listRunTask;
}

//清理所有任务
void run_timed_task::ClearTask() {
    QMutexLocker locker(&m_mutex);

    m_listTask.clear();
}

//是否退出
bool run_timed_task::GetExit() {
    return m_bExit;
}

//线程执行主要函数,找出要提醒的消息。并且执行提醒
void run_timed_task::CheckTimedTask(PFuncCallbackSS cb, void *pArgs) {
    QMutexLocker locker(&m_mutex);
    //得到当前日期和时间
    QDateTime dateTimeCur = QDateTime::currentDateTime();
    QString qstrDatetimeCur = dateTimeCur.toString("yyyy-MM-dd HH:mm:ss");
    //遍历
    for (auto iter = m_listTask.begin(); iter != m_listTask.end(); iter++) {
        QDateTime dateTimeLastRun = QDateTime::fromString(iter->qstrLastRunTime, "yyyy-MM-dd HH:mm:ss");
        bool bRunAndUpdate = false;
        //检查执行方式
        switch (iter->nRunType) {
        case TTRT_EVERY_ONCE: {
            QDateTime dateTimeRunTime = QDateTime::fromString(iter->qstrRunTime, "yyyy-MM-dd HH:mm:ss");
            //从未执行,且当前时间已经大于执行时间了
            if (iter->qstrLastRunTime == iter->qstrRunTime
                    && (dateTimeCur > dateTimeRunTime)){
                 bRunAndUpdate = true;
            }
        }
            break;
        case TTRT_EVERY_MINUTE: {
            if (dateTimeLastRun.secsTo(dateTimeCur) >= 60) {
                 bRunAndUpdate = true;
            }
        }
            break;
        case TTRT_EVERY_HOUR: {
            if (dateTimeLastRun.secsTo(dateTimeCur) >= 60 * 60) {
               bRunAndUpdate = true;
            }
        }
            break;
        case TTRT_EVERY_HALF_HOUR: {
            if (dateTimeLastRun.secsTo(dateTimeCur) >= 30 * 60) {
              bRunAndUpdate = true;
            }
        }
        case TTRT_EVERY_DAY: {
            if (dateTimeLastRun.daysTo(dateTimeCur) >= 1) {
              bRunAndUpdate = true;
            }
        }
            break;
        case TTRT_EVERY_WEEK: {
            QDate dateCur = dateTimeCur.date();
            QDate dateLastRun = dateTimeLastRun.date();
            if ((dateCur.dayOfWeek() == dateLastRun.dayOfWeek())
                    && (dateCur.weekNumber() > dateLastRun.weekNumber())
                    ) {
                bRunAndUpdate = true;
            }
        }
            break;
        case TTRT_EVERY_MONTH: {
            QDate dateCur = dateTimeCur.date();
            QDate dateLastRun = dateTimeLastRun.date();
            if (dateCur.daysInMonth() == dateLastRun.daysInMonth()
                    && (dateCur.month() > dateLastRun.month())
                    ) {
                 bRunAndUpdate = true;
            }
        }
            break;
        case TTRT_EVERY_YEAR: {
            QDate dateCur = dateTimeCur.date();
            QDate dateLastRun = dateTimeLastRun.date();
            if (dateCur.daysInYear() == dateLastRun.daysInYear()
                    && (dateCur.year() > dateLastRun.year())
                    ) {
               bRunAndUpdate = true;
            }
        }
            break;
        case TTRT_EVERY_SPECIAL_INTERVAL:{
            //当前时间
            QDateTime dateTimeCur = QDateTime::currentDateTime();
            qint64 q64curTimestamp = dateTimeCur.toMSecsSinceEpoch();

            //最后一次i执行时间
            QDateTime dateTimeLastRunTime = QDateTime::fromString(iter->qstrLastRunTime, "yyyy-MM-dd HH:mm:ss");
            qint64 q64LastTimestamp = dateTimeLastRunTime.toMSecsSinceEpoch();

            //时间间隔
            // QDateTime dateTimeRunTime(QDate::currentDate(), QTime::fromString("HH:mm:ss", iter->qstrRunTime));// = QDateTime::fromString(iter->qstrRunTime, "yyyy-MM-dd HH:mm:ss");
            // qint64 q64RunTimestamp = dateTimeRunTime.toMSecsSinceEpoch();
            QTime qTimeInterval = QTime::fromString(iter->qstrRunTime, QStringLiteral("HH:mm:ss"));
            int nInterval = qTimeInterval.msecsSinceStartOfDay();
            //时间是否到了
            if (q64curTimestamp - q64LastTimestamp > nInterval) {
                bRunAndUpdate = true;
            }
        }
            break;
        default:
            break;
        }
        //可以执行
        if (bRunAndUpdate) {
            iter->qstrLastRunTime = qstrDatetimeCur;
            //修改时间
            sqlite_tools::get_sqlite_tools().ChangeLastRunTime(iter->nId, qstrDatetimeCur);
            //记录日志
            QString qstrPromptLog = qstrDatetimeCur + QString(QStringLiteral(", 执行了定时任务名：")) + iter->qstrName;
            sqlite_tools::get_sqlite_tools().AddPromptLog(qstrDatetimeCur, qstrPromptLog);
            //包装为了把消息发给QT主线程
            if (cb) cb(*iter, pArgs);
        }
    }
}

//槽， 发现需要新处理的定时任务
void run_timed_task::OnPushTimedTask(const TimedTaskItem &item) {
    m_pRBPopupWin->PushTask(item);
}
