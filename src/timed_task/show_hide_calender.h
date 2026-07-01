#ifndef SHOW_HIDE_CALENDER_H
#define SHOW_HIDE_CALENDER_H

#include <QDialog>

namespace Ui {
class show_hide_calender;
}

class show_hide_calender : public QDialog
{
    Q_OBJECT

public:
    explicit show_hide_calender(QWidget *parent = nullptr);
    ~show_hide_calender();
    //触发日历选择
    void OnSelectionChanged();
    //确认结果
    void BtnOK();

signals:
    //信号，发送给父窗口。设置时间
    void SendSignalTimedTaskSelect(QString);
private:
    Ui::show_hide_calender *ui;
};

#endif // SHOW_HIDE_CALENDER_H
