#ifndef MAIN_UPDATE_BOTTOM_H
#define MAIN_UPDATE_BOTTOM_H

#include <QLabel>
#include <QObject>
#include <QTimer>
#include <QPropertyAnimation>
#include <QPoint>

class main_update_bottom : public QLabel
{
     Q_OBJECT

    enum MUB_STATUS {
        SCROOL_NORMAL, //正常显示状态
        SCROLL_UP_PRIV_START,//上一条滚动显示开始状态
        SCROLL_UP_PRIV_ING,//上一条滚动显示中状态
        SCROLL_UP_NEXT_START,//下一条滚动显示开始状态
        SCROLL_UP_NEXT_ING,//下一条滚动显示中状态

        SCROLL_COUNT
    };

public:
    main_update_bottom(QWidget *parent=nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    //提示日志更新
    void OnTimerUpdateBottom();
    //上一条滚动出去的效果
    void OnTimerPrivScrollUp();
    //下一条滚动进来的效果
    void OnTimerNextScrollUp();
    //上一条消失
    void PrivHide();
    //下一条显示
    void NextShow();
    //得到居中开始位置
    void GetCenterStartPos();

private:
    QLabel *m_pLabel;
    //定时器更新提示消息
    QTimer m_timerUpdateBottom;
    //上一条滚动出去的效果
    QTimer m_timerPrivScrollUp;
    //下一条滚动进来的效果
    QTimer m_timerNextScrollUp;
    //动画效果,实现滚动
    QPropertyAnimation *m_pAnimation;
    //记录上一次要显示的字符串，为了实现滚动
    QString m_qstrPrivText;
    //记录当前要显示的字符串
    QString m_qstrCurText;
    // 当前的显示状态
    MUB_STATUS m_nStatus;
    //显示的位置
    QPoint m_ptShowStartPos;

};

#endif // MAIN_UPDATE_BOTTOM_H
