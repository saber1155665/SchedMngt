#include "main_update_bottom.h"
#include "sqlite_tools.h"


#include <QObject>
#include <QPainter>

//滚动分成多少次
#define SCROLL_TIMES (3)
//滚动时候，滚动的总时间间隔
#define SCROLL_INTERVAL_TOTAL 5000
//更新一次的时间间隔
#define UPDATE_BOTTOM_INTERVAL (5000)

main_update_bottom::main_update_bottom(QWidget *parent)
    : QLabel(parent)
    , m_nStatus(SCROOL_NORMAL)
{
    //每五秒触发一次
    connect(&m_timerUpdateBottom, &QTimer::timeout, this, &main_update_bottom::OnTimerUpdateBottom);
    m_timerUpdateBottom.start(UPDATE_BOTTOM_INTERVAL); // 开启
    //
    connect(&m_timerPrivScrollUp, &QTimer::timeout, this, &main_update_bottom::OnTimerPrivScrollUp);
    connect(&m_timerNextScrollUp, &QTimer::timeout, this, &main_update_bottom::OnTimerNextScrollUp);

}


//提示日志更新
void main_update_bottom::OnTimerUpdateBottom() {
    QList<PromptLogItem> listPromptLog;
    sqlite_tools::get_sqlite_tools().QueryPromptLogLastFive(listPromptLog);

    auto pFuncChangeShowText = [&](QString &qstrNew) {
        m_qstrPrivText = m_qstrCurText;
        m_qstrCurText = qstrNew;
    };

    //找出上次显示的位置
    bool bFind = false;
    for (auto iter = listPromptLog.begin(); iter != listPromptLog.end(); iter++) {
        //找到了
        if (iter->qstrLogInfo == m_qstrCurText) {
            //移动到下一条
            iter++;
            if (iter != listPromptLog.end()) {
                pFuncChangeShowText(iter->qstrLogInfo);
            } else {//回到第一条
                pFuncChangeShowText(listPromptLog.begin()->qstrLogInfo);
            }
            bFind = true;
            break;
        }
    }
    //没找到。数据库更新了最少五条记录，或者第一次运行没有内容
    if (!bFind && listPromptLog.size() > 0) {
         pFuncChangeShowText(listPromptLog.begin()->qstrLogInfo);
    }
    //开始滚动
    m_timerUpdateBottom.stop();//停止更新定时器
    //开启滚动定时器,先隐藏上一条
    GetCenterStartPos();
    m_timerPrivScrollUp.start( SCROLL_INTERVAL_TOTAL / SCROLL_TIMES);
}

//上一条消失
void main_update_bottom::PrivHide() {
    m_pAnimation->setDuration(5000);
    // 设置动画的起始和结束值
    m_pAnimation->setStartValue(QPoint(0,0));
    m_pAnimation->setEndValue(QPoint(0, -m_pLabel->height()));

    // 启动动画
    m_pAnimation->start();
}

//下一条显示
void main_update_bottom::NextShow() {

}


//void main_update_bottom::paintEvent(QPaintEvent *paint) {
//    QPainter qPainter(this);
//    QFontMetricsF fm = qPainter.fontMetrics();
//
//    qPainter.setRenderHint(QPainter::TextAntialiasing); // 文字抗锯齿
//
//    // 1. 扣除内边距，获取真实可绘制区域
//    QMargins margins = contentsMargins();
//    QRectF drawRect = rect().adjusted(
//        margins.left(), margins.top(),
//        -margins.right(), -margins.bottom()
//    );
//    // 3. 计算文字精准包围矩形
//    QRectF textRect = fm.boundingRect(drawRect, Qt::AlignLeft, m_qstrCurText);
//    // 4. 计算垂直居中偏移：可用区域中点 - 文字矩形中点
//    qreal yOffset = drawRect.center().y() - textRect.center().y();
//
//    if ((SCROLL_UP_PRIV_START == m_nStatus)
//            || (SCROLL_UP_PRIV_ING == m_nStatus)) {
//        if (SCROLL_UP_PRIV_START == m_nStatus) {//第一次文字肯定居中的
//            m_dMoveOffset = yOffset;
//            m_nStatus = SCROLL_UP_PRIV_ING;
//        } else {
//            m_dMoveOffset = m_dMoveOffset - (yOffset / SCROLL_TIMES);
//        }
//        qPainter.drawText(drawRect.x(), m_dMoveOffset, m_qstrCurText);
//     } else if ((SCROLL_UP_NEXT_START == m_nStatus)
//             || (SCROLL_UP_NEXT_ING == m_nStatus)) {
//        if (SCROLL_UP_NEXT_START == m_nStatus) {//第一次文字肯定居中的
//            m_dMoveOffset = drawRect.y();
//            m_nStatus = SCROLL_UP_NEXT_ING;
//        } else {
//            m_dMoveOffset = m_dMoveOffset - (yOffset / SCROLL_TIMES);
//        }
//        qPainter.drawText(drawRect.x(), m_dMoveOffset, m_qstrCurText);
//    } else if (SCROOL_NORMAL == m_nStatus) {
//        if (yOffset < 0) {//文本高度大于窗口高度，就只能直接显示了
//            // 设置水平+垂直居中（核心代码）
//            setAlignment(Qt::AlignCenter);
//            setText(m_qstrCurText);
//        } else {
//            qPainter.drawText(drawRect.x(), yOffset + fm.ascent(), m_qstrCurText);
//        }
//    }
//
//
//
//}

void main_update_bottom::paintEvent(QPaintEvent *paint) {
    QPainter qPainter(this);

    qPainter.drawText(m_ptShowStartPos.x(), m_ptShowStartPos.y(), m_qstrCurText);
}

//滚动时候的日志
void main_update_bottom::OnTimerPrivScrollUp() {
    QPainter qPainter(this);
    QFontMetricsF fm = qPainter.fontMetrics();
    qreal dMoveOffset = 0.0;

    qPainter.setRenderHint(QPainter::TextAntialiasing); // 文字抗锯齿

    //检查是否已经移动完成
    if (m_ptShowStartPos.y() <= 0) {
        //关闭上一条滚出定时器
        m_timerPrivScrollUp.stop();
        //开启下一条滚入定时器
        m_timerNextScrollUp.start( SCROLL_INTERVAL_TOTAL / SCROLL_TIMES);
        update();
        return;
    }

    // 1. 扣除内边距，获取真实可绘制区域
    QMargins margins = contentsMargins();
    QRectF drawRect = rect().adjusted(
        margins.left(), margins.top(),
        -margins.right(), -margins.bottom()
    );
    // 3. 计算文字精准包围矩形
    QRectF textRect = fm.boundingRect(drawRect, Qt::AlignLeft, m_qstrCurText);
    // 4. 计算垂直居中偏移：可用区域中点 - 文字矩形中点
    qreal yOffset = drawRect.center().y() - textRect.center().y();

    if (dMoveOffset == yOffset + fm.ascent()) {//第一次文字肯定居中的
        dMoveOffset = yOffset;
    } else {
        dMoveOffset = m_ptShowStartPos.y() - (yOffset / SCROLL_TIMES);
    }
    m_ptShowStartPos = QPoint(drawRect.x(), dMoveOffset);
    update();
}


//下一条滚动进来的效果
void main_update_bottom::OnTimerNextScrollUp() {
    QPainter qPainter(this);
    QFontMetricsF fm = qPainter.fontMetrics();
    qreal dMoveOffset = 0.0;

    qPainter.setRenderHint(QPainter::TextAntialiasing); // 文字抗锯齿

    // 1. 扣除内边距，获取真实可绘制区域
    QMargins margins = contentsMargins();
    QRectF drawRect = rect().adjusted(
        margins.left(), margins.top(),
        -margins.right(), -margins.bottom()
    );
    // 3. 计算文字精准包围矩形
    QRectF textRect = fm.boundingRect(drawRect, Qt::AlignLeft, m_qstrCurText);
    // 4. 计算垂直居中偏移：可用区域中点 - 文字矩形中点
    qreal yOffset = drawRect.center().y() - textRect.center().y();

    if (dMoveOffset == yOffset + fm.ascent()) {//第一次文字肯定居中的
        dMoveOffset = drawRect.y();
    } else {
        dMoveOffset = m_ptShowStartPos.y() - (yOffset / SCROLL_TIMES);
        //移动到了中间了
        if (dMoveOffset < yOffset + fm.ascent()) {
            //结束滚入
            m_timerNextScrollUp.stop();
            //开启下一轮循环
            m_timerUpdateBottom.start(UPDATE_BOTTOM_INTERVAL);
        }
    }
    m_ptShowStartPos = QPoint(drawRect.x(), dMoveOffset);
    update();
}

//得到居中开始位置
void main_update_bottom::GetCenterStartPos() {
    QPainter qPainter(this);
    QFontMetricsF fm = qPainter.fontMetrics();
    qreal dMoveOffset = 0.0;

    qPainter.setRenderHint(QPainter::TextAntialiasing); // 文字抗锯齿

    // 1. 扣除内边距，获取真实可绘制区域
    QMargins margins = contentsMargins();
    QRectF drawRect = rect().adjusted(
        margins.left(), margins.top(),
        -margins.right(), -margins.bottom()
    );
    // 3. 计算文字精准包围矩形
    QRectF textRect = fm.boundingRect(drawRect, Qt::AlignLeft, m_qstrCurText);
    // 4. 计算垂直居中偏移：可用区域中点 - 文字矩形中点
    qreal yOffset = drawRect.center().y() - textRect.center().y();

    m_ptShowStartPos = QPoint(drawRect.x(), yOffset + fm.ascent());
}
