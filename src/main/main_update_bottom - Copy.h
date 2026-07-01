#ifndef MAIN_UPDATE_BOTTOM_H
#define MAIN_UPDATE_BOTTOM_H

#include <QLabel>
#include <QObject>
#include <QTimer>
#include <QPropertyAnimation>

class main_bottom_label : public QObject
{
     Q_OBJECT
public:
    main_bottom_label(QLabel *pLabel);

private:
    //提示日志更新
    void OnTimerUpdateBottom();
    //上一条消失
    void PrivHide();
    //下一条显示
    void NextShow();

private:
    QLabel *m_pLabel;
    //定时器更新提示消息
    QTimer m_timerUpdateBottom;
    //动画效果,实现滚动
    QPropertyAnimation *m_pAnimation;
};

#endif // MAIN_UPDATE_BOTTOM_H
