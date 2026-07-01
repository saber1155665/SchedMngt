#ifndef SCHED_MNGT_TABLE_ITEM_H
#define SCHED_MNGT_TABLE_ITEM_H

#include <QWidget>
#include <QList>

namespace Ui {
class sched_mngt_table_item;
}


struct DayTaskItem {
    QString qstrTaskName; //任务名
    bool bCompleteTask; //任务是否完成
};

class sched_mngt_table_item : public QWidget
{
    Q_OBJECT

public:
    explicit sched_mngt_table_item(QWidget *parent = nullptr);
    ~sched_mngt_table_item();

    //设置当月日期
    void SetDate(const QDate &date);
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    //显示当天的日期和农历
    void UpdateCurrentDate(const QDate &date);
    //增加每天的定时任务
    void SetDayTask(const QDate &date);
private:
    Ui::sched_mngt_table_item *ui;
};

#endif // SCHED_MNGT_TABLE_ITEM_H
