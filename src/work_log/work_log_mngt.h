#ifndef WORK_LOG_MNGT_H
#define WORK_LOG_MNGT_H

#include <QDialog>

namespace Ui {
class work_log_mngt;
}

class work_log_mngt : public QDialog
{
    Q_OBJECT

public:
    explicit work_log_mngt(QWidget *parent = nullptr);
    ~work_log_mngt();


private slots:
    //信号槽，添加了新的工作日志
    void OnAddNewWorkLog();

private:
    //刷新数据
    void RefreshTbData();
    //退出窗口
    void Exit();
    //添加项
    void Add();
    //页面+1
    void NextPage();
    //页面-1
    void PrivPage();
private:
    Ui::work_log_mngt *ui;
    //页数,注意页面是从0开始的
    int m_nPage;
    //页面总数
    int m_nPageCount;
};

#endif // WORK_LOG_MNGT_H
