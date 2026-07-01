#include "sqlite_tools.h"
#include "sched_mngt_utils.h"

#include <QString>
#include <QApplication>
#include <QStringLiteral>
#include <QDate>
#include <QDateTime>

sqlite_tools::sqlite_tools()
{
    QString appDir = QCoreApplication::applicationDirPath();
    appDir.append(QStringLiteral("/test2.db"));
    try {
        appDir.replace("/", "\\");
        std::string strDBPath = GetStdUtf8String(appDir);
        m_pDatabase = new SQLite::Database(strDBPath.c_str(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        //建立表
        CreateDataBase();
    }  catch (...) {
        m_pDatabase = NULL;
    }

}

//单例模式
sqlite_tools & sqlite_tools::get_sqlite_tools() {
    static sqlite_tools tools;
    return tools;
}

//创建表
bool sqlite_tools::CreateDataBase() {
    //工作计划
    m_pDatabase->exec("CREATE TABLE IF NOT EXISTS tb_timed_tasks(id INTEGER PRIMARY KEY"
                      ", name TEXT"
                      ", run_type INTEGER"
                      ", run_time TEXT"
                      ", note TEXT"
                      ", last_run_time TEXT"
                      ", last_run_timestamp INTEGER"
                      ", create_time TEXT)"
                      );

    //工作日志
    m_pDatabase->exec("CREATE TABLE IF NOT EXISTS tbl_work_log(id INTEGER PRIMARY KEY"
                      ", create_time TEXT"
                      ", create_timestamp INTEGER"
                      ",  note TEXT"
                      ")");

    //日志信息
    m_pDatabase->exec("CREATE TABLE IF NOT EXISTS tbl_prompt_log(id INTEGER PRIMARY KEY"
                      ", create_time TEXT"
                      ",  log_info TEXT"
                      ")");

    return true;
}


//关闭数据库
bool sqlite_tools::CloseDb() {
    //关闭
    delete m_pDatabase;
    m_pDatabase = nullptr;

    return true;
}

//查询定时任务
bool sqlite_tools::QueryTimedTask(int nStart, int nCount, QList<TimedTaskItem> &listTimedTask) {
    QString qstrQuery = QString(
                            QStringLiteral("SELECT id, name, run_type, run_time, note,"
                                           "last_run_time, last_run_timestamp, create_time  FROM tb_timed_tasks  LIMIT %1 OFFSET %2"))
                        .arg(nCount).arg(nStart);
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
           TimedTaskItem timedTaskItem;
           //获取id
           timedTaskItem.nId = query.getColumn("id").getInt();
           //获取名
           std::string qstrName = query.getColumn("name").getString();
           timedTaskItem.qstrName = QString::fromUtf8(qstrName.c_str());
           //获取运行方式
           timedTaskItem.nRunType = (TimedTaskRunType)(query.getColumn("run_type").getInt());
           //获取运行时间
           std::string qstrRunTime = query.getColumn("run_time").getString();
           timedTaskItem.qstrRunTime = QString::fromUtf8(qstrRunTime.c_str());
           //获取备注
           std::string qstrNote = query.getColumn("note").getString();
           timedTaskItem.qstrNote = QString::fromUtf8(qstrNote.c_str());
           //获取最后一次执行时间
           std::string qstrLastRunTime = query.getColumn("last_run_time").getString();
           timedTaskItem.qstrLastRunTime = QString::fromUtf8(qstrLastRunTime.c_str());
           timedTaskItem.i64LastRunTimestamp = query.getColumn("last_run_timestamp").getInt64();
           //获取创建时间
           std::string qstrCreateTime = query.getColumn("create_time").getString();
           timedTaskItem.qstrCreateTime = QString::fromUtf8(qstrCreateTime.c_str());

            //添加项
           listTimedTask.push_back(timedTaskItem);
        }
    }  catch (...) {
        return false;
    }

    return true;
}

//添加定时任务
bool sqlite_tools::AddTimedTask(QString &qstrName, QString &qstrNote, QString &qstrRunTime, TimedTaskRunType nRunType) {
    QDate selectedDate = QDate::currentDate();
    QTime selectTime = QTime::currentTime();
    QString qstrData = selectedDate.toString("yyyy-MM-dd");
    QString qstrTime = selectTime.toString("HH:mm:ss");
    QString qstrCurrentTime = qstrData + " " + qstrTime;
    QString qstrAdd = QString("INSERT INTO tb_timed_tasks(name, run_type, run_time, note, last_run_time, last_run_timestamp, create_time) "
                              "VALUES(?, ?, ?, ?, ?, ?, ?)");

    try {
        SQLite::Statement   insert(*m_pDatabase,GetStdUtf8String(qstrAdd).c_str());
        insert.bind(1, GetStdUtf8String(qstrName).c_str());
        insert.bind(2, nRunType);
        insert.bind(3, GetStdUtf8String(qstrRunTime).c_str());
        insert.bind(4, GetStdUtf8String(qstrNote).c_str());
        insert.bind(5, GetStdUtf8String(qstrRunTime).c_str());
        QDateTime dateTime = QDateTime::fromString(qstrRunTime, "yyyy-MM-dd HH:mm:ss");
        insert.bind(6, dateTime.toMSecsSinceEpoch());
        insert.bind(7, GetStdUtf8String(qstrCurrentTime).c_str());
        insert.exec();
    }  catch (...) {
        return false;
    }
    return true;
}

//获取定时任务总数
qint64 sqlite_tools::GetTimedTaskCount() {
    qint64 i64Count = 0;
    QString qstrQuery = QString( QStringLiteral("SELECT count(*) AS cnt FROM tb_timed_tasks "));
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
           //获取id
           i64Count = query.getColumn("cnt").getInt64();
        }
    }  catch (...) {
        return false;
    }
    return i64Count;
}

//删除项
bool sqlite_tools::DeleteTimedTask(int nId) {
    QString qstrDel = QString("DELETE FROM tb_timed_tasks where id=%1")
            .arg(nId);
    int nRst = m_pDatabase->exec(GetStdUtf8String(qstrDel).c_str());
    return nRst;
}

//修改最后一次执行时间
bool sqlite_tools::ChangeLastRunTime(int nID, QString &qstrLastRunTime) {
    QString qstrUpdate = QString("UPDATE tb_timed_tasks SET  last_run_time=?,last_run_timestamp=? where id=%2")
            .arg(qstrLastRunTime)
            .arg(nID);
    try {
        SQLite::Statement   update(*m_pDatabase,GetStdUtf8String(qstrUpdate).c_str());
        update.bind(1, GetStdUtf8String(qstrLastRunTime).c_str());
        QDateTime dateTime = QDateTime::fromString(qstrLastRunTime, "yyyy-MM-dd HH:mm:ss");
        update.bind(2, dateTime.toMSecsSinceEpoch());
        update.exec();
    }  catch (...) {
        return false;
    }
    return true;
}

//获取指定日期的工作日志
QList<TimedTaskItem> sqlite_tools::QueryTimedTaskByDatetime(const QDate &date) {
    QList<TimedTaskItem> listTimedTask;

    //得到今日开始的毫秒
    QDateTime dateTimeCur(date);
    int64_t i64Cur = dateTimeCur.toMSecsSinceEpoch();
    //得到后一日开始的毫秒
    QDate dateNext = date.addDays(+1);
    QDateTime dateTimeNext(dateNext);
    int64_t i64Next = dateTimeNext.toMSecsSinceEpoch();

    QString qstrQuery = QString(
                            QStringLiteral("SELECT id, name, run_type, run_time, note, last_run_time, last_run_timestamp, create_time"
                                           "  FROM tb_timed_tasks WHERE last_run_timestamp>=? AND last_run_timestamp <?"));
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        query.bind(1, i64Cur);
        query.bind(2, i64Next);
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
            TimedTaskItem timedTaskItem;
            //获取id
            timedTaskItem.nId = query.getColumn("id").getInt();
            //获取名
            std::string qstrName = query.getColumn("name").getString();
            timedTaskItem.qstrName = QString::fromUtf8(qstrName.c_str());
            //获取运行方式
            timedTaskItem.nRunType = (TimedTaskRunType)(query.getColumn("run_type").getInt());
            //获取运行时间
            std::string qstrRunTime = query.getColumn("run_time").getString();
            timedTaskItem.qstrRunTime = QString::fromUtf8(qstrRunTime.c_str());
            //获取备注
            std::string qstrNote = query.getColumn("note").getString();
            timedTaskItem.qstrNote = QString::fromUtf8(qstrNote.c_str());
            //获取最后一次执行时间
            std::string qstrLastRunTime = query.getColumn("last_run_time").getString();
            timedTaskItem.qstrLastRunTime = QString::fromUtf8(qstrLastRunTime.c_str());
            timedTaskItem.i64LastRunTimestamp = query.getColumn("last_run_timestamp").getInt64();
            //获取创建时间
            std::string qstrCreateTime = query.getColumn("create_time").getString();
            timedTaskItem.qstrCreateTime = QString::fromUtf8(qstrCreateTime.c_str());

             //添加项
            listTimedTask.push_back(timedTaskItem);
        }
    }  catch (...) {
        return listTimedTask;
    }
    return listTimedTask;
}

bool sqlite_tools::QueryWorkLog(int nStart, int nCount, QList<WorkLogItem> &listWorkLog) {
    QString qstrQuery = QString(
                            QStringLiteral("SELECT id, create_time, create_timestamp, note"
                                           "  FROM tbl_work_log  LIMIT %1 OFFSET %2"))
                        .arg(nCount).arg(nStart);
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
           WorkLogItem workLogItem;
           //获取id
           workLogItem.nId = query.getColumn("id").getInt();
           //创建时间
           std::string qstrCreateTime = query.getColumn("create_time").getString();
           workLogItem.qstrCreateTime = QString::fromUtf8(qstrCreateTime.c_str());
           //创建时间时间戳
           workLogItem.i64CreateTimestamp = query.getColumn("create_timestamp").getInt64();
           //工作日志
           std::string qstrNote = query.getColumn("note").getString();
           workLogItem.qstrNote = QString::fromUtf8(qstrNote.c_str());

           //添加项
          listWorkLog.push_back(workLogItem);
        }
    }  catch (...) {
        return false;
    }
    return true;
}

//新增工作日志
bool sqlite_tools::AddWorkLog(QString &qstrCreateTime, QString &qstrNote) {
    QString qstrAdd = QString("INSERT INTO tbl_work_log(create_time, create_timestamp, note) "
                              " VALUES (?, ?, ?)");
    try {
        SQLite::Statement   insert(*m_pDatabase,GetStdUtf8String(qstrAdd).c_str());
        insert.bind(1, GetStdUtf8String(qstrCreateTime).c_str());
        QDateTime dateTime = QDateTime::fromString(qstrCreateTime, "yyyy-MM-dd HH:mm:ss");
        insert.bind(2, dateTime.toMSecsSinceEpoch());
        insert.bind(3, GetStdUtf8String(qstrNote).c_str());
        insert.exec();
    }  catch (...) {
        return false;
    }
    return true;
}

//获取工作总数
qint64 sqlite_tools::GetWorkLogCount() {
    qint64 i64Count = 0;
    QString qstrQuery = QString( QStringLiteral("SELECT count(*) AS cnt FROM tbl_work_log "));
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
           //获取id
           i64Count = query.getColumn("cnt").getInt64();
        }
    }  catch (...) {
        return false;
    }
    return i64Count;
}

//获取指定日期的工作日志
WorkLogItem sqlite_tools::QueryWorkLogByDatetime(const QDate &date) {
    WorkLogItem workLogItem = {-1, "", -1, ""};
    //得到今日开始的毫秒
    QDateTime dateTimeCur(date);
    int64_t i64Cur = dateTimeCur.toMSecsSinceEpoch();
    //得到后一日开始的毫秒
    QDate dateNext = date.addDays(+1);
    QDateTime dateTimeNext(dateNext);
    int64_t i64Next = dateTimeNext.toMSecsSinceEpoch();

    QString qstrQuery = QString(
                            QStringLiteral("SELECT id, create_time, create_timestamp, note"
                                           "  FROM tbl_work_log WHERE create_timestamp>=? AND create_timestamp <?"));
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        query.bind(1, i64Cur);
        query.bind(2, i64Next);
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
           //获取id
           workLogItem.nId = query.getColumn("id").getInt();
           //创建时间
           std::string qstrCreateTime = query.getColumn("create_time").getString();
           workLogItem.qstrCreateTime = QString::fromUtf8(qstrCreateTime.c_str());
           //创建时间时间戳
           workLogItem.i64CreateTimestamp = query.getColumn("create_timestamp").getInt64();
           //工作日志
           std::string qstrNote = query.getColumn("note").getString();
           workLogItem.qstrNote = QString::fromUtf8(qstrNote.c_str());
        }
    }  catch (...) {
        return workLogItem;
    }
    return workLogItem;


}


//查询日志
bool sqlite_tools::QueryPromptLog(int nStart, int nCount, QList<PromptLogItem> &listLogInfo) {
    QString qstrQuery = QString(
                            QStringLiteral("SELECT id, create_time, log_info"
                                           "  FROM tbl_prompt_log  LIMIT %1 OFFSET %2"))
                        .arg(nCount).arg(nStart);
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
           PromptLogItem logInfoItem;
           //获取id
           logInfoItem.nId = query.getColumn("id").getInt();
           //logInfoItem
           std::string qstrCreateTime = query.getColumn("create_time").getString();
           logInfoItem.qstrCreateTime = QString::fromUtf8(qstrCreateTime.c_str());
           //工作日志
           std::string qstrLogInfo = query.getColumn("log_info").getString();
           logInfoItem.qstrLogInfo = QString::fromUtf8(qstrLogInfo.c_str());

           //添加项
          listLogInfo.push_back(logInfoItem);
        }
    }  catch (...) {
        return false;
    }
    return true;
}

//查询日志最后的五条
bool sqlite_tools::QueryPromptLogLastFive(QList<PromptLogItem> &listLogInfo) {
    QString qstrQuery = QString(
                            QStringLiteral("SELECT id, create_time, log_info"
                                           "  FROM tbl_prompt_log ORDER BY create_time DESC  LIMIT 5 OFFSET 0 "))
                        ;
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
           PromptLogItem logInfoItem;
           //获取id
           logInfoItem.nId = query.getColumn("id").getInt();
           //logInfoItem
           std::string qstrCreateTime = query.getColumn("create_time").getString();
           logInfoItem.qstrCreateTime = QString::fromUtf8(qstrCreateTime.c_str());
           //工作日志
           std::string qstrLogInfo = query.getColumn("log_info").getString();
           logInfoItem.qstrLogInfo = QString::fromUtf8(qstrLogInfo.c_str());

           //添加项
          listLogInfo.push_back(logInfoItem);
        }
    }  catch (...) {
        return false;
    }
    return true;
}

//新增工作日志
bool sqlite_tools::AddPromptLog(QString &qstrCreateTime, QString &qstrLogInfo) {
    QString qstrAdd = QString("INSERT INTO tbl_prompt_log(create_time, log_info) "
                              " VALUES (?, ?)");
    try {
        SQLite::Statement   insert(*m_pDatabase,GetStdUtf8String(qstrAdd).c_str());
        insert.bind(1, GetStdUtf8String(qstrCreateTime).c_str());
        insert.bind(2, GetStdUtf8String(qstrLogInfo).c_str());
        insert.exec();
    }  catch (...) {
        return false;
    }
    return true;
}

//获取日志总数
qint64 sqlite_tools::GetPromptLogCount() {
    qint64 i64Count = 0;
    QString qstrQuery = QString( QStringLiteral("SELECT count(*) AS cnt FROM tbl_prompt_log "));
    try {
        SQLite::Statement   query(*m_pDatabase,GetStdUtf8String(qstrQuery).c_str());
        // Loop to execute the query step by step, to get one a row of results at a time
        while (query.executeStep()) {
           //获取id
           i64Count = query.getColumn("cnt").getInt64();
        }
    }  catch (...) {
        return false;
    }
    return i64Count;
}
