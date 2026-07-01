#include "work_log.h"
#include "ui_work_log.h"
#include "show_hide_calender.h"
#include "sqlite_tools.h"
#include "sched_mngt_message_box.h"

work_log::work_log(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::work_log)
{
    ui->setupUi(this);
    //关联
    connect(ui->btn_drop_down, &QPushButton::clicked, this, &work_log::ShowDropdownDateTime);
    connect(ui->btn_add, &QPushButton::clicked, this, &work_log::Add);
}

work_log::~work_log()
{
    delete ui;
}

//显示获取时间窗口
void work_log::ShowDropdownDateTime() {
    show_hide_calender *p_show_hide_calender = new show_hide_calender(this);
    p_show_hide_calender->setWindowModality(Qt::WindowModal);
    p_show_hide_calender->show();
    connect(p_show_hide_calender, SIGNAL(SendSignalTimedTaskSelect(QString)), this, SLOT(TimedTaskSelect(QString)));
}

//信号，获取选择时间的窗口的结果
void work_log::TimedTaskSelect(QString qstrDateTime) {
    ui->line_edit_date_time->setText(qstrDateTime);

}

//退出窗口
void work_log::Exit() {
    //退出
    close();
}
//添加项
void work_log::Add() {
    //得到名字
    QString qstrDateTime = ui->line_edit_date_time->text();
    //得到名字
    QString qstrNote = ui->textEdit->toPlainText();
    //添加到数据库
    if (!sqlite_tools::get_sqlite_tools().AddWorkLog(qstrDateTime, qstrNote)) {
         sched_mngt_message_box::question(QStringLiteral("添加失败"), sched_mngt_message_box::MBT_ERROR);
         return;
    }
    //提示
    sched_mngt_message_box::question(QStringLiteral("添加完成"), sched_mngt_message_box::MBT_INFO);
    //刷新页面
    emit SendAddNewWorkLog();
    //退出
    Exit();
}
