#ifndef BREATHING_PROGRESS_H
#define BREATHING_PROGRESS_H

#include <QProgressBar>
#include <QPainter>
#include <QTimer>
#include <QLinearGradient>
#include <QTime>
#include <QtMath>

class BreathingProgressBar : public QProgressBar {
    Q_OBJECT
public:
    explicit BreathingProgressBar(QWidget *parent = nullptr) : QProgressBar(parent) {
        setStyleSheet("QProgressBar { border: none; text-align: center; color: white; }");
        setMinimum(0);
        setMaximum(100);
        setValue(0);

        // 颜色偏移定时器（控制彩虹色循环）
        m_colorTimer = new QTimer(this);
        connect(m_colorTimer, &QTimer::timeout, this, [&]() {
            m_colorOffset = (m_colorOffset + 1) % 360; // 0-359循环
            update(); // 重绘
        });
        m_colorTimer->start(30); // 30ms 更新颜色

        // 呼吸效果定时器（控制透明度）
        m_breathTimer = new QTimer(this);
        connect(m_breathTimer, &QTimer::timeout, this, [&]() {
            m_breathValue = (m_breathValue + 1) % 100;
            update();
        });
        m_breathTimer->start(50);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 1. 绘制背景
        QRect bgRect = rect();
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(30, 30, 30)); // 深色背景
        painter.drawRoundedRect(bgRect, 15, 15);

        // 2. 计算进度区域
        int progressWidth = static_cast<int>(value() * 1.0 / maximum() * bgRect.width());
        QRect progressRect = bgRect.adjusted(2, 2, progressWidth - bgRect.width() - 2, -2);

        // 3. 计算呼吸透明度（0.5-1.0 循环）
        qreal alpha = 0.5 + (qSin(m_breathValue * 0.0628) + 1) / 4; // 正弦曲线实现呼吸

        // 4. 绘制彩虹渐变+呼吸效果
        QLinearGradient gradient(progressRect.left(), progressRect.top(), progressRect.right(), progressRect.bottom());
        // 彩虹色标（随m_colorOffset偏移）
        for (int i = 0; i < 7; i++) {
            int hue = (m_colorOffset + i * 51) % 360; // 7种颜色，间隔51度
            QColor color = QColor::fromHsv(hue, 255, 255, alpha * 255);
            gradient.setColorAt(i / 6.0, color);
        }

        painter.setBrush(gradient);
        painter.drawRoundedRect(progressRect, 13, 13);

        // 5. 绘制文本
        painter.setPen(QColor(255, 255, 255, alpha * 255));
        painter.drawText(bgRect, Qt::AlignCenter, QString("%1%").arg(value()));
    }

private:
    QTimer *m_colorTimer = nullptr;   // 颜色循环定时器
    QTimer *m_breathTimer = nullptr;  // 呼吸效果定时器
    int m_colorOffset = 0;            // 颜色偏移量（0-359）
    int m_breathValue = 0;            // 呼吸效果值（0-99）
};
#endif //BREATHING_PROGRESS_H
