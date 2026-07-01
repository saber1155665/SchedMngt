#include "right_bottom_popupwindow.h"
#include "ui_right_bottom_popupwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QString>
#include <QStringLiteral>

//隐藏的最大值
#define MAX_HIDE_TIME (20)

right_bottom_popupwindow::right_bottom_popupwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::right_bottom_popupwindow)
{
    ui->setupUi(this);
    //关联槽
    connect(ui->btn_hide, &QPushButton::clicked, this, &right_bottom_popupwindow::OkHide);
    connect(ui->btn_close, &QPushButton::clicked, this, &right_bottom_popupwindow::OkHide);
    //设置无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //设置顶层
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    //每两秒触发一次
    connect(&m_timer, &QTimer::timeout, this, &right_bottom_popupwindow::TimerCheckTimedTask);
    m_timer.setSingleShot(true); // 设置定时器为单次触发
    m_timer.start(2000); // 2000毫秒后触发
    //定时退出按钮
    connect(&m_timerAutoHide, &QTimer::timeout, this, &right_bottom_popupwindow::AutoHideWin);
    // 初始化动画
    m_pAnimation = new QPropertyAnimation(this, "geometry");
    m_pAnimation->setDuration(2000); // 动画持续时间2秒

    //默认隐藏该窗口
    hide();
}

right_bottom_popupwindow::~right_bottom_popupwindow()
{
    delete ui;
}

//增加任务
void right_bottom_popupwindow::PushTask(const TimedTaskItem &item) {
    QMutexLocker locker(&m_mutex);
    m_listTask.push_back(item);
    m_timer.start(2000);
}
//获取任务
bool right_bottom_popupwindow::PopTask(TimedTaskItem &item) {
    QMutexLocker locker(&m_mutex);
    if (0 == m_listTask.size()) {
        return false;
    }
    item = m_listTask.back();
    m_listTask.pop_back();
    return true;
}
//检查一次是否有任务要显示，如果有就显示
void right_bottom_popupwindow::TimerCheckTimedTask() {
    TimedTaskItem item;
    if (PopTask(item)) {
        //设置内容
        ui->label_title->setText(QStringLiteral("您有新的定时任务"));
        ui->label_info_title->setText(item.qstrName);
        ui->label_info->setText(item.qstrNote);
        //显示出来
        show();
        //设置按钮的自动隐藏
        m_nHideTime = MAX_HIDE_TIME;
        //每一秒触发一次
        m_timerAutoHide.start(1000);
        //动画
        startAnimation();
        //停止
        m_timer.stop();
    } else {
        //
        hide();
        //
        m_timer.stop();
        m_timer.start(2000); // 2000毫秒后触发
    }
}

void right_bottom_popupwindow::startAnimation() {
        // 获取屏幕尺寸
        QRect screenGeometry = QApplication::desktop()->availableGeometry();

        // 初始位置：右下角
        int startX = screenGeometry.width() - width();
        int startY = screenGeometry.height();

        // 目标位置：右下角向上移动200像素
        int endX = startX;
        int endY = screenGeometry.height() - height() - 200;

        // 设置动画的起始和结束值
        m_pAnimation->setStartValue(QRect(startX, startY, width(), height()));
        m_pAnimation->setEndValue(QRect(endX, endY, width(), height()));

        // 启动动画
        m_pAnimation->start();
}

//启动隐藏
void right_bottom_popupwindow::AutoHideWin() {
    m_nHideTime--;
    if (m_nHideTime <= 0) {
        //结束定时器
        m_timerAutoHide.stop();
        //检查是否有的数据
        OkHide();
    }
    QString qstrInfo = QString(QStringLiteral("%1秒自动隐藏")).arg(m_nHideTime);
    ui->btn_hide->setText(qstrInfo);
}

//用户自己点击了确认
void right_bottom_popupwindow::OkHide() {
    //
    m_nHideTime = 0;
    //先隐藏
    hide();
    //是否还有新的
    TimerCheckTimedTask();
}
