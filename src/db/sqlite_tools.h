#ifndef SQLITE_TOOLS_H
#define SQLITE_TOOLS_H

#include "SQLiteCpp.h"

#include <QString>
#include <QList>

enum TimedTaskRunType {
    TTRT_EVERY_UNKNOWN,//默认值
    TTRT_EVERY_ONCE,//仅执行一次
    TTRT_EVERY_MINUTE,//每分钟一次
    TTRT_EVERY_HALF_HOUR, //每半小时一次
    TTRT_EVERY_HOUR, //每小时一次
    TTRT_EVERY_DAY, //每天一次
    TTRT_EVERY_WEEK,//每周一次
    TTRT_EVERY_MONTH,//每月一次
    TTRT_EVERY_YEAR,//每年一次
    TTRT_EVERY_SPECIAL_INTERVAL,//指定时间间隔

    TTRT_COUNT
};

//查询定时任务的item
struct TimedTaskItem {
    int nId;
    QString qstrName;
    QString qstrNote;
    QString qstrRunTime;
    TimedTaskRunType nRunType;
    QString qstrLastRunTime;
    qint64 i64LastRunTimestamp;
    QString qstrCreateTime;
};

//查询工作日志的item
struct WorkLogItem {
    int nId;
    QString qstrCreateTime;
    int64_t i64CreateTimestamp;
    QString qstrNote;
};


//查询日志记录的item
struct PromptLogItem {
    int nId;
    QString qstrCreateTime;
    QString qstrLogInfo;
};

class sqlite_tools
{
public:
    sqlite_tools();
    //单例模式
    static sqlite_tools & get_sqlite_tools();
    //创建表
    bool CreateDataBase();
    //关闭数据库
    bool CloseDb();

    ////////////////////////////定时任务////////////////////////
    //查询定时任务
    bool QueryTimedTask(int nStart, int nCount, QList<TimedTaskItem> &listTimedTask);
    //添加定时任务
    bool AddTimedTask(QString &qstrName, QString &qstrNote, QString &qstrRunTime, TimedTaskRunType nRunType);
    //获取定时任务总数
    qint64 GetTimedTaskCount();
    //删除项
    bool DeleteTimedTask(int nId);
    //修改最后一次执行时间
    bool ChangeLastRunTime(int nID, QString &qstrLastRunTime);
    //获取指定日期的工作日志
    QList<TimedTaskItem> QueryTimedTaskByDatetime(const QDate &date);

    ////////////////////////////工作日志////////////////////////
    //查询工作日志
    bool QueryWorkLog(int nStart, int nCount, QList<WorkLogItem> &listWorkLog);
    //新增工作日志
    bool AddWorkLog(QString &qstrCreateTime, QString &qstrNote);
    //获取工作总数
    qint64 GetWorkLogCount();
    //获取指定日期的工作日志
    WorkLogItem QueryWorkLogByDatetime(const QDate &date);

    ////////////////////////////记录日志，方便查询哪些执行成功了////////////////////////
    //查询日志
    bool QueryPromptLog(int nStart, int nCount, QList<PromptLogItem> &listLogInfo);
    //查询日志最后的五条
    bool QueryPromptLogLastFive(QList<PromptLogItem> &listLogInfo);
    //新增工作日志
    bool AddPromptLog(QString &qstrCreateTime, QString &qstrLogInfo);
    //获取日志总数
    qint64 GetPromptLogCount();
private:
    SQLite::Database *m_pDatabase;
};

#endif // SQLITE_TOOLS_H
