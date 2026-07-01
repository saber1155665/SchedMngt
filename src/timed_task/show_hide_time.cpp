#include "show_hide_time.h"
#include "ui_show_hide_time.h"

ShowHideTime::ShowHideTime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowHideTime)
{
    ui->setupUi(this);

    //关联槽
    connect(ui->push_button_ok, &QPushButton::clicked, this, &ShowHideTime::BtnOK);

    //设置时间格式
    ui->time_edit_interval->setDisplayFormat("HH:mm:ss");
    //将时间改为当前时间
    //ui->time_edit_interval->setTime(QTime::currentTime());
}

ShowHideTime::~ShowHideTime()
{
    delete ui;
}

//确认结果
void ShowHideTime::BtnOK() {
    //加上选择是时间
    QTime selectTime = ui->time_edit_interval->time();
    //格式化日期
    QString qstrTime = selectTime.toString("HH:mm:ss");

    emit SendSignalTimedTaskSelect(qstrTime);
    close();
}
