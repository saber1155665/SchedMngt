#include "show_hide_calender.h"
#include "ui_show_hide_calender.h"

show_hide_calender::show_hide_calender(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::show_hide_calender)
{
    ui->setupUi(this);
    //关联槽
    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, &show_hide_calender::OnSelectionChanged);
    connect(ui->btn_timed_task_select, &QPushButton::clicked, this, &show_hide_calender::BtnOK);
    connect(ui->time_edit, &QTimeEdit::timeChanged, this, &show_hide_calender::OnSelectionChanged);
    //设置时间格式
    ui->time_edit->setDisplayFormat("HH:mm:ss");
    //将时间改为当前时间
    ui->time_edit->setTime(QTime::currentTime());
    //设置默认值
    QDate selectedDate = QDate::currentDate();
    QTime selectTime = QTime::currentTime();
    QString qstrData = selectedDate.toString("yyyy-MM-dd");
    QString qstrTime = selectTime.toString("HH:mm:ss");
    QString qstrDateTime = qstrData + " " + qstrTime;
    ui->text_edit_date_time->setText(qstrDateTime);
}

show_hide_calender::~show_hide_calender()
{
    delete ui;
}
//触发日历选择
void show_hide_calender::OnSelectionChanged() {
    //得到选择的时间
    QDate selectedDate = ui->calendarWidget->selectedDate();
    //加上选择是时间
    QTime selectTime = ui->time_edit->time();
    //格式化日期
    QString qstrData = selectedDate.toString("yyyy-MM-dd");
    QString qstrTime = selectTime.toString("HH:mm:ss");
    QString qstrDateTime = qstrData + " " + qstrTime;
    ui->text_edit_date_time->setText(qstrDateTime);
}

//确认结果
void show_hide_calender::BtnOK() {
    QString qstrDateTime = ui->text_edit_date_time->toPlainText();
    emit SendSignalTimedTaskSelect(qstrDateTime);
    close();
}

