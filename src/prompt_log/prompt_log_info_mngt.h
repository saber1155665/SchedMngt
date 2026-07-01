#ifndef PROMPT_LOG_INFO_MNGT_H
#define PROMPT_LOG_INFO_MNGT_H

#include <QDialog>
#include <QMap>

namespace Ui {
class prompt_log_info_mngt;
}

class prompt_log_info_mngt : public QDialog
{
    Q_OBJECT

public:
    explicit prompt_log_info_mngt(QWidget *parent = nullptr);
    ~prompt_log_info_mngt();

private:
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
private:
    Ui::prompt_log_info_mngt *ui;
    //页数,注意页面是从0开始的
    int m_nPage;
    //页面总数
    int m_nPageCount;
};

#endif // PROMPT_LOG_INFO_MNGT_H
