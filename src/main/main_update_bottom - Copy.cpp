#include "main_update_bottom.h"
#include "sqlite_tools.h"


#include <QObject>

main_bottom_label::main_bottom_label(QLabel *pLabel)
    : m_pLabel(pLabel)
{
    //每五秒触发一次
    connect(&m_timerUpdateBottom, &QTimer::timeout, this, &main_bottom_label::OnTimerUpdateBottom);
    m_timerUpdateBottom.start(5000); // 5000毫秒后触发

    m_pAnimation = new QPropertyAnimation(m_pLabel, "text");

}


//提示日志更新
void main_bottom_label::OnTimerUpdateBottom() {
    PrivHide();
    return ;
    QList<PromptLogItem> listPromptLog;
    sqlite_tools::get_sqlite_tools().QueryPromptLogLastFive(listPromptLog);

    //找出上次显示的位置
    bool bFind = false;
    QString qstrText = m_pLabel->text();
    for (auto iter = listPromptLog.begin(); iter != listPromptLog.end(); iter++) {
        //找到了
        if (iter->qstrLogInfo == qstrText) {
            //移动到下一条
            iter++;
            if (iter != listPromptLog.end()) {
                m_pLabel->setText(iter->qstrLogInfo);
            } else {//回到第一条
                m_pLabel->setText(listPromptLog.begin()->qstrLogInfo);
            }
            bFind = true;
            break;
        }
    }
    //没找到。数据库更新了最少五条记录，或者第一次运行没有内容
    if (!bFind && listPromptLog.size() > 0) {
         m_pLabel->setText(listPromptLog.begin()->qstrLogInfo);
    }
}

//上一条消失
void main_bottom_label::PrivHide() {
    m_pAnimation->setDuration(5000);
    // 设置动画的起始和结束值
    m_pAnimation->setStartValue(QPoint(0,0));
    m_pAnimation->setEndValue(QPoint(0, -m_pLabel->height()));

    // 启动动画
    m_pAnimation->start();
}

//下一条显示
void main_bottom_label::NextShow() {

}

