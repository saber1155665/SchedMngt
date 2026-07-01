#ifndef WORK_LOG_H
#define WORK_LOG_H

#include <QDialog>

namespace Ui {
class work_log;
}

class work_log : public QDialog
{
    Q_OBJECT

public:
    explicit work_log(QWidget *parent = nullptr);
    ~work_log();

signals:
    //信号，发送给父窗口。添加了新的工作日志
    void SendAddNewWorkLog();

private:
    //显示获取时间窗口
    void ShowDropdownDateTime();
    //退出窗口
    void Exit();
    //添加项
    void Add();
private slots:
    //信号，获取选择时间的窗口的结果
    void TimedTaskSelect(QString qstrDateTime);

private:
    Ui::work_log *ui;
};

#endif // WORK_LOG_H
