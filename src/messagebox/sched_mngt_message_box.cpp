#include "sched_mngt_message_box.h"
#include "ui_sched_mngt_message_box.h"

#include <QStringLiteral>

sched_mngt_message_box::sched_mngt_message_box(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sched_mngt_message_box)
{
    ui->setupUi(this);
    //设置无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //关联槽
    connect(ui->btn_close, &QPushButton::clicked, this, &sched_mngt_message_box::OnMsgOK);
    connect(ui->btn_ok, &QPushButton::clicked, this, &sched_mngt_message_box::OnMsgOK);
    //默认标题
    ui->label_title->setText(QStringLiteral("消息"));
    //自动释放
    setAttribute(Qt::WA_DeleteOnClose);
}

sched_mngt_message_box::~sched_mngt_message_box()
{
    delete ui;
}


//点击了确认
void sched_mngt_message_box::OnMsgOK() {
    close();
}


//设置消息
void sched_mngt_message_box::SetMsgInfo(const QString &qstrInfo, const QString &qstrTitle) {
    ui->label_msg_info->setText(qstrInfo);
    if (!qstrTitle.isEmpty()) {
       ui->label_title->setText(qstrTitle);
    }
}

//提示消息
void sched_mngt_message_box::Info(const QString &qstr) {
    SetMsgInfo(qstr, QStringLiteral("消息"));
}

//错误消息
void sched_mngt_message_box::Error(const QString &qstr) {
     SetMsgInfo(qstr, QStringLiteral("错误"));
}

//警告消息
void sched_mngt_message_box::Warning(const QString &qstr) {
     SetMsgInfo(qstr, QStringLiteral("警告"));
}


//创建提示消息
void sched_mngt_message_box::question(const QString &qstrInfo, const QString &qstrTitle) {
    sched_mngt_message_box *p_sched_mngt_message_box = new sched_mngt_message_box();
    p_sched_mngt_message_box->setWindowModality(Qt::WindowModal);
    p_sched_mngt_message_box->SetMsgInfo(qstrInfo, qstrTitle);
    p_sched_mngt_message_box->show();
}

void sched_mngt_message_box::question(const QString &qstrInfo, MsgBoxType type) {
    sched_mngt_message_box *p_sched_mngt_message_box = new sched_mngt_message_box();
    p_sched_mngt_message_box->setWindowModality(Qt::WindowModal);
    switch (type) {
    case MBT_INFO : {
        p_sched_mngt_message_box->Info(qstrInfo);
        break;
    }
    case MBT_ERROR : {
        p_sched_mngt_message_box->Error(qstrInfo);
        break;
    }
    case MBT_WARN : {
        p_sched_mngt_message_box->Warning(qstrInfo);
        break;
    }
    }
    p_sched_mngt_message_box->show();
}
