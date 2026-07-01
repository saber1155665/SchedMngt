#ifndef SHOW_HIDE_TIME_H
#define SHOW_HIDE_TIME_H

#include <QDialog>

namespace Ui {
class ShowHideTime;
}

class ShowHideTime : public QDialog
{
    Q_OBJECT

public:
    explicit ShowHideTime(QWidget *parent = nullptr);
    ~ShowHideTime();


    //确认结果
    void BtnOK();

signals:
    //信号，发送给父窗口。设置时间
    void SendSignalTimedTaskSelect(QString);

private:
    Ui::ShowHideTime *ui;
};

#endif // SHOW_HIDE_TIME_H
