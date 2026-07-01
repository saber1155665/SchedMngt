#include "timed_task.h"
#include "ui_timed_task.h"
#include "show_hide_calender.h"
#include "sched_mngt_message_box.h"
#include "show_hide_time.h"

#include <QString>
#include <QStringList>
#include <QStringLiteral>
#include <QMap>

timed_task::timed_task(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::timed_task)
{
    ui->setupUi(this);
    //设置无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //关联
    connect(ui->btn_close, &QPushButton::clicked, this, &timed_task::Exit);
    connect(ui->btn_cancel, &QPushButton::clicked, this, &timed_task::Exit);
    connect(ui->btn_add,   &QPushButton::clicked, this, &timed_task::Add);
    connect(ui->btn_drop_down,   &QPushButton::clicked, this, &timed_task::DropDownCalender);
    //添加项目
    m_qmapRunType = InitRunTypeAndString();
    AddCombBoxRunType();
    //添加
}

timed_task::~timed_task()
{
    delete ui;
}

//退出窗口
void timed_task::Exit() {
    timed_task::close();
}

//添加项
void timed_task::Add() {
    //得到名字
    QString qstrName = ui->text_edit_name->toPlainText();
    //得到名字
    QString qstrNote = ui->text_edit_note->toPlainText();
    //得到类型
    TimedTaskRunType nRunType = m_qmapRunType[ui->combo_box_run_type->currentText()];
    //得到时间
    QString qstrRunTime = ui->text_edit_run_time->toPlainText();
    //添加到数据库
    if (!sqlite_tools::get_sqlite_tools().AddTimedTask(qstrName, qstrNote, qstrRunTime, nRunType)) {
         sched_mngt_message_box::question(QStringLiteral("添加失败"), sched_mngt_message_box::MBT_ERROR);
         return;
    }
    //提示
    sched_mngt_message_box::question(QStringLiteral("添加完成"), sched_mngt_message_box::MBT_INFO);
    //刷新页面
    emit SendAddNewTimedTask();
    //退出
    Exit();
}

//添加运行方式
QMap<QString ,TimedTaskRunType> timed_task::InitRunTypeAndString() {
    QMap<QString ,TimedTaskRunType> qmapRunType;

    qmapRunType[QStringLiteral("仅执行一次")] = TTRT_EVERY_ONCE;
    qmapRunType[QStringLiteral("每分钟一次")] = TTRT_EVERY_MINUTE;
    qmapRunType[QStringLiteral("每半小时一次")] = TTRT_EVERY_HALF_HOUR;
    qmapRunType[QStringLiteral("每小时一次")] = TTRT_EVERY_HOUR;
    qmapRunType[QStringLiteral("每天一次")] = TTRT_EVERY_DAY;
    qmapRunType[QStringLiteral("每周一次")] = TTRT_EVERY_WEEK;
    qmapRunType[QStringLiteral("每月一次")] = TTRT_EVERY_MONTH;
    qmapRunType[QStringLiteral("每年一次")] = TTRT_EVERY_YEAR;
    qmapRunType[QStringLiteral("指定间隔(秒)")] = TTRT_EVERY_SPECIAL_INTERVAL;
    return qmapRunType;
}

//添加运行方式
void timed_task::AddCombBoxRunType() {
     QStringList stringList;

    for (auto iter = m_qmapRunType.begin(); iter != m_qmapRunType.end(); iter++) {
        stringList << iter.key();
    }
    ui->combo_box_run_type->addItems(stringList);
}

//下拉日历
void timed_task::DropDownCalender() {
    //得到类型
    TimedTaskRunType nRunType = m_qmapRunType[ui->combo_box_run_type->currentText()];

    //非指定间隔，那么就要显示日历
    if (TTRT_EVERY_SPECIAL_INTERVAL != nRunType) {
        show_hide_calender *p_show_hide_calender = new show_hide_calender(this);
        p_show_hide_calender->setWindowModality(Qt::WindowModal);
        p_show_hide_calender->show();
        connect(p_show_hide_calender, SIGNAL(SendSignalTimedTaskSelect(QString)), this, SLOT(TimedTaskSelect(QString)));
    } else {
        //指定间隔就显示时间
        ShowHideTime *p_show_hide_time = new ShowHideTime(this);
        p_show_hide_time->setWindowModality(Qt::WindowModal);
        p_show_hide_time->show();
        connect(p_show_hide_time, SIGNAL(SendSignalTimedTaskSelect(QString)), this, SLOT(TimedTaskSelect(QString)));
    }

}

//信号，获取选择时间的窗口的结果
void timed_task::TimedTaskSelect(QString qstrDateTime) {
    ui->text_edit_run_time->setText(qstrDateTime);

}
