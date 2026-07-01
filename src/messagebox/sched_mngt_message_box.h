#ifndef SCHED_MNGT_MESSAGE_BOX_H
#define SCHED_MNGT_MESSAGE_BOX_H

#include <QDialog>

namespace Ui {
class sched_mngt_message_box;
}

class sched_mngt_message_box : public QDialog
{
    Q_OBJECT

public:
    explicit sched_mngt_message_box(QWidget *parent = nullptr);
    ~sched_mngt_message_box();

    enum MsgBoxType {
        MBT_INFO,
        MBT_ERROR,
        MBT_WARN,

        MBT_COUNT
    };

    //创建提示消息
    static void question(const QString &qstrInfo, MsgBoxType type);
    static void question(const QString &qstrInfo, const QString &qstrTitle);
private:
    //提示消息
    void Info(const QString &qstr);

    //错误消息
    void Error(const QString &qstr);

    //警告消息
    void Warning(const QString &qstr);

    //设置消息
    void SetMsgInfo(const QString &qstrInfo, const QString &qstrTitle = QString());

private:
    //点击了确认
    void OnMsgOK();

private:
    Ui::sched_mngt_message_box *ui;
};

#endif // SCHED_MNGT_MESSAGE_BOX_H
