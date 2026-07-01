#ifndef TIMED_TASK_MNGT_H
#define TIMED_TASK_MNGT_H

#include "run_timed_task.h"

#include <QDialog>
#include <QMap>

namespace Ui {
class timed_task_mngt;
}

class timed_task_mngt : public QDialog
{
    Q_OBJECT

    enum ComboxBoxActionType{
        CBAT_SPACE,  //不操作
        CBAT_DELETE, //删除项
        CBAT_CHANGE, //修改内容

        CBAT_COUNT
    };

public:
    explicit timed_task_mngt(run_timed_task *pRunTimedTask = nullptr, QWidget *parent = nullptr);
    ~timed_task_mngt();

private slots:
    //信号槽，添加了新的定时任务
    void OnAddNewTimedTask();

private:
    //添加新的定时任务
    void AddNewTimedTask();

    //刷新数据
    void RefreshTbData();
    //退出窗口
    void Exit();
    //页面+1
    void NextPage();
    //页面-1
    void PrivPage();
    //手动在页面数那里修改了数字
    void OnChangePage();
    //键盘按下消息
    void keyPressEvent(QKeyEvent *event);
    //编辑或者删除的combox选项触发消息
    void OnComboxBoxChange(int index);
private:
    Ui::timed_task_mngt *ui;
    //页数,注意页面是从0开始的
    int m_nPage;
    //页面总数
    int m_nPageCount;
    //字符串对应的操作
    QMap<QString, ComboxBoxActionType> m_qmapComboxBoxAction;
    //显示定时器提醒
    run_timed_task *m_pRunTimedTask;
};

#endif // TIMED_TASK_MNGT_H
