#include "sched_mngt_table_item.h"
#include "ui_sched_mngt_table_item.h"
#include "calendar.h"
#include "sqlite_tools.h"

#include <qstring.h>
#include <QDate>
#include <QLabel>

sched_mngt_table_item::sched_mngt_table_item(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sched_mngt_table_item)
{
    ui->setupUi(this);
    // 关键1：启用样式背景渲染（必加！）
    this->setAttribute(Qt::WA_StyledBackground, true);
}

sched_mngt_table_item::~sched_mngt_table_item()
{
    delete ui;
}

//设置当月日期
void sched_mngt_table_item::SetDate(const QDate &date) {
    UpdateCurrentDate(date);

    SetDayTask(date);
}

//显示当天的日期和农历
void sched_mngt_table_item::UpdateCurrentDate(const QDate &date) {
    ChineseLunarDate lunarDate;
    lunarDate.date.y = date.year();
    lunarDate.date.m = date.month();
    lunarDate.date.d = date.day();

    chineseLunarDate(&lunarDate);
    QString qstrTerms =(ST_UNKNOWN == lunarDate.lunar.term) ? QStringLiteral("") :  QString::fromWCharArray(solarTermsToString(lunarDate.lunar.term));
    QString qstrFestival = QString::fromWCharArray(lunarFestivalToString(lunarDate.lunar.festival));
    QString qstrLunarDay = QString::fromWCharArray(lunarMDayToString(lunarDate.lunar.d));
    if (1 == lunarDate.lunar.d) {//初一就改为显示月份
        qstrLunarDay = QString::fromWCharArray(lunarMonthToString(lunarDate.lunar.m));
    }
    QString qstr = QString("%1 %2 %3 %4").arg(date.day()).arg(qstrLunarDay).arg(qstrTerms).arg(qstrFestival);
    ui->label_day_lunar_day->setText(qstr);
}

//增加每天的定时任务
void sched_mngt_table_item::SetDayTask(const QDate &date) {
    QString qstrTooltips;
    //添加工作日志
    WorkLogItem workLogItem = sqlite_tools::get_sqlite_tools().QueryWorkLogByDatetime(date);
    if (workLogItem.qstrNote.isEmpty()) {
        //没有工作日志就不显示
        ui->vertical_layout_work_log->parentWidget()->hide();
    } else {
        //添加
        QLabel *pLabelWorkLog = new QLabel(this);
        pLabelWorkLog->setText(workLogItem.qstrNote);
        pLabelWorkLog->setFixedHeight(24);
        ui->vertical_layout_work_log->addWidget(pLabelWorkLog);
        // 创建 Spacer
        QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui->vertical_layout_work_log->addItem(spacer);
        ui->vertical_layout_work_log->addStretch(1);
        qstrTooltips = QStringLiteral("工作日志:") + workLogItem.qstrNote + " ";
    }

    //添加今日的定时任务
    auto listTimedTask = sqlite_tools::get_sqlite_tools().QueryTimedTaskByDatetime(date);
    if (0 == listTimedTask.size()) {
        //没有定时任务就不显示
        ui->vertical_layout_task_item->parentWidget()->hide();
    } else {
        qstrTooltips = qstrTooltips + QStringLiteral("定时任务:");
        for (auto iter = listTimedTask.begin(); iter != listTimedTask.end(); iter++) {
            //添加
            QLabel *pLabelTimedTask = new QLabel(this);
            pLabelTimedTask->setText(iter->qstrName);
            pLabelTimedTask->setFixedHeight(24);
            ui->vertical_layout_task_item->addWidget(pLabelTimedTask);
            qstrTooltips += iter->qstrName + ",";
        }
        // 创建 Spacer
        QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui->vertical_layout_task_item->addItem(spacer);
        ui->vertical_layout_task_item->addStretch(1);
    }
    //设置提示信息;
    if (qstrTooltips.isEmpty()) {
         qstrTooltips = QStringLiteral("无");
    }
    setToolTip(qstrTooltips);
}

void sched_mngt_table_item::resizeEvent(QResizeEvent *event) {
    QRect rectThis = this->geometry();
    rectThis = QRect(2, 2, rectThis.width() - 2 - 2, rectThis.height() - 2 - 2);
    //日期,固定高度，自适应长度
    ui->label_day_lunar_day->setGeometry(QRect(0, 0, rectThis.width(), 24));
    //剩下的空间给定时任务和工作日志平分
    if (ui->vertical_layout_task_item->parentWidget()->isVisible()
            && ui->vertical_layout_work_log->parentWidget()->isVisible()) {
        int nHalfHeight = (rectThis.height() - 24) / 2;
        ui->vertical_layout_task_item->parentWidget()->setGeometry(QRect(0, 24, rectThis.width(), nHalfHeight));
        ui->vertical_layout_work_log->parentWidget()->setGeometry(QRect(0, 24 + nHalfHeight , rectThis.width(), nHalfHeight));
    } else if (ui->vertical_layout_task_item->parentWidget()->isVisible()) {//仅显示定时任务
        ui->vertical_layout_task_item->parentWidget()->setGeometry(QRect(0, 24, rectThis.width(), rectThis.height() - 24));
    } else if (ui->vertical_layout_work_log->parentWidget()->isVisible()) {//仅显示工作日志
        ui->vertical_layout_work_log->parentWidget()->setGeometry(QRect(0, 24, rectThis.width(), rectThis.height() - 24));
    } else{
    }

    return QWidget::resizeEvent(event);
}
