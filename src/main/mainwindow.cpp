#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sched_mngt_table_item.h"
#include "timed_task_mngt.h"
#include "run_timed_task.h"
#include "work_log_mngt.h"
#include "prompt_log_info_mngt.h"
#include "sched_mngt_utils.h"
#include "fast_start_develop_toolbox.h"

#include <QString>
#include <QStringList>
#include <QDate>

//日历表页数
#define TABLE_COL_SIZE 7
//日历表行数
#define TABLE_ROW_SIZE 6

// 定义边缘检测的阈值（像素）
#define EDGE_WIDTH 4

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_isDragging(false)
    , m_isResizing(false)
{
    ui->setupUi(this);
    //保证窗口可以随时捕获鼠标位置，方便边缘缩放
    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);

    setAttribute(Qt::WA_TransparentForMouseEvents, false); // 确保不透明鼠标事件
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    setAttribute(Qt::WA_NativeWindow, true);

    //关联槽
    connect(ui->btn_close, &QPushButton::clicked, this, &MainWindow::Exit);
    connect(ui->btn_min, &QPushButton::clicked, this, &MainWindow::MinSizeWindow);
    connect(ui->btn_timed_task, &QToolButton::clicked, this, &MainWindow::ShowTimedTaskMgmt);
    connect(ui->btn_work_log, &QToolButton::clicked, this, &MainWindow::ShowWorkLogMgmt);
    connect(ui->btn_log_info, &QToolButton::clicked, this, &MainWindow::PromptShowInfo);
    connect(ui->btn_develop_toolbox, &QToolButton::clicked, this, &MainWindow::DevelopToolbox);

    QStringList strListHeader;
    strListHeader << QStringLiteral("星期一")
                  << QStringLiteral("星期二")
                  << QStringLiteral("星期三")
                  << QStringLiteral("星期四")
                  << QStringLiteral("星期五")
                  << QStringLiteral("星期六")
                  << QStringLiteral("星期日");
    ui->sched_mngt_table->setColumnCount(TABLE_COL_SIZE); //列数为7
    ui->sched_mngt_table->setRowCount(TABLE_ROW_SIZE);//行数为6
    ui->sched_mngt_table->setHorizontalHeaderLabels(strListHeader);
    ui->sched_mngt_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->sched_mngt_table->setAutoFillBackground(false);
    //设置高宽自适应
    for (int i = 0; i < strListHeader.size(); i++) {
         ui->sched_mngt_table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    for (int i = 0; i < TABLE_ROW_SIZE; i++) {
        ui->sched_mngt_table->verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    //添加日历item
    for (int i = 0; i < TABLE_ROW_SIZE; i++) {
        for (int j = 0; j < strListHeader.size(); j++) {
             ui->sched_mngt_table->setCellWidget(i, j,  static_cast<QWidget*>(new sched_mngt_table_item(this)));
        }
    }
    //计算冰设置页面对应的日期
    CalcPageDate(0);

    //设置托盘
    m_customTray = new custom_tray_icon(this);
    connect(m_customTray, &custom_tray_icon::CustomTrayMsg, this, &MainWindow::OnCustomTrayClicked);
    m_customTray->show();
    //启动定时任务执行
    m_pRunTimedTask = new run_timed_task(this);
    m_pRunTimedTask->RefreshTaskList();

    //初始化系统快捷键
    fast_start_develop_toolbox *pFastStart = new fast_start_develop_toolbox(nullptr);
    connect(pFastStart, &fast_start_develop_toolbox::OnFastStartDevelopToolboxSignals, this, &MainWindow::OnFastStartDevelopToolbox);
    m_ptrDevelopToolbox.reset(new develop_toolbox_main());

    //启动隐藏主窗口
    hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//计算指定页的月份日期表
void MainWindow::CalcPageDate(int nPage) {
    // 创建当前日期
    QDate currentDate = QDate::currentDate();
    //获取指定的月份
    QDate specifyDate = currentDate.addMonths(nPage);
    //得到年月日
    int nYear = 0;
    int nMonth = 0;
    int nDay = 0;
    specifyDate.getDate(&nYear, &nMonth, &nDay);
    //得到该月的第一天
    QDate monthFirstDayDate = specifyDate.addDays(-nDay + 1);
    //得到该月第一天是星期几
    int nWeek = monthFirstDayDate.dayOfWeek();
    //先添加上月的日子，并且设置成灰色
    QDate pageStartDate = monthFirstDayDate.addDays(-nWeek + 1);
    for  (int i  = 0; i < TABLE_ROW_SIZE; i++) {
        for  (int j  = 0; j < TABLE_COL_SIZE; j++) {
            QDate tmpDate = pageStartDate.addDays(i * TABLE_COL_SIZE + j);
            sched_mngt_table_item *p_sched_mngt_table_item = static_cast<sched_mngt_table_item*>(ui->sched_mngt_table->cellWidget(i, j));
            if (p_sched_mngt_table_item) {
                p_sched_mngt_table_item->SetDate(tmpDate);
                //非本月就设置成灰色
                if (monthFirstDayDate.month() != tmpDate.month()) {
                     p_sched_mngt_table_item->setDisabled(true);
                } else {//都是本月的日志了
                    //当天应该设置醒目颜色
                    if (currentDate.day() == tmpDate.day()) {
                        p_sched_mngt_table_item->setStyleSheet(QStringLiteral("QWidget {background-color: rgb(135, 206, 235);}"));
                    }
                }
            }
        }
    }
}

//退出窗口
void MainWindow::Exit() {
    hide();
}

//窗口最小化
void MainWindow::MinSizeWindow() {
     showMinimized();
}

//双击/单击托盘事件
int MainWindow::OnCustomTrayClicked(custom_tray_icon::TrayMsg msg) {
    if (custom_tray_icon::TrayMsg::TM_DoubleClick == msg)
    {
        if (!this->isVisible()) {
            this->showNormal();
        }
    }
    return 0;
}


//显示添加定时任务窗口
void MainWindow::ShowTimedTaskMgmt() {
    timed_task_mngt *p_timed_task_mngt = new timed_task_mngt(m_pRunTimedTask, this);
    p_timed_task_mngt->setWindowModality(Qt::WindowModal);
    p_timed_task_mngt->show();
}

//显示工作日志
void MainWindow::ShowWorkLogMgmt() {
    work_log_mngt *p_work_log_mngt = new work_log_mngt(this);
    p_work_log_mngt->setWindowModality(Qt::WindowModal);
    p_work_log_mngt->show();
}


//显示日志信息
void MainWindow::PromptShowInfo() {
    prompt_log_info_mngt *p_prompt_log_info_mngt = new prompt_log_info_mngt(this);
    p_prompt_log_info_mngt->setWindowModality(Qt::WindowModal);
    p_prompt_log_info_mngt->show();
}

//显示日志信息
void MainWindow::DevelopToolbox() {
    m_ptrDevelopToolbox->show();
}

//槽， 快捷输入启动
void MainWindow::OnFastStartDevelopToolbox(const TreeItemAction action) {
    m_ptrDevelopToolbox->OnFastStartDevelopToolbox(action);
    m_ptrDevelopToolbox->show();
}


// 鼠标按下事件
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 1. 判断是否在标题栏区域（这里假设整个窗口上方50px为标题栏，可自定义）
        QRect titleBarRect = ui->horizontal_layout_header->geometry();
        if (titleBarRect.contains(event->pos()))
        {
            m_isDragging = true;
            m_dragStartPos = this->pos();
            m_mouseStartPos = getGlobalMousePos(event);
            event->accept();
        }

        // 2. 判断是否在缩放区域
        if (m_resizeDir != None)
        {
            m_isResizing = true;
            m_windowStartRect = this->geometry();
            m_resizeStartPos = getGlobalMousePos(event);
            event->accept();
        }


    }
    QWidget::mousePressEvent(event);
}

// 鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint currentPos = event->pos();
    QPoint globalPos = getGlobalMousePos(event);

    // 1. 处理缩放
    if (m_isResizing)
    {
        QRect newRect = m_windowStartRect;
        int dx = globalPos.x() - m_resizeStartPos.x();
        int dy = globalPos.y() - m_resizeStartPos.y();

        switch (m_resizeDir)
        {
        case Left:
            newRect.setLeft(m_windowStartRect.left() + dx);
            // 限制窗口宽度不小于最小宽度
            if (newRect.width() < minimumWidth())
                newRect.setLeft(newRect.right() - minimumWidth());
            break;
        case Right:
            newRect.setRight(m_windowStartRect.right() + dx);
            if (newRect.width() < minimumWidth())
                newRect.setRight(newRect.left() + minimumWidth());
            break;
        case Bottom:
            newRect.setBottom(m_windowStartRect.bottom() + dy);
            if (newRect.height() < minimumHeight())
                newRect.setBottom(newRect.top() + minimumHeight());
            break;
        default:
            break;
        }

        // 更新窗口几何形状
        setGeometry(newRect);
        event->accept();
        return;
    }

    // 2. 处理拖动
    if (m_isDragging)
    {
        QPoint offset = globalPos - m_mouseStartPos;
        move(m_dragStartPos + offset);
        event->accept();
        return;
    }

    // 3. 检测鼠标是否在缩放区域，更新光标和缩放方向
    m_resizeDir = None;
    // 左边缘
    if (currentPos.x() <= EDGE_WIDTH)
        m_resizeDir = Left;
    // 右边缘
    else if (currentPos.x() >= width() - EDGE_WIDTH)
        m_resizeDir = Right;
    // 下边缘
    else if (currentPos.y() >= height() - EDGE_WIDTH)
        m_resizeDir = Bottom;

    // 设置光标样式
    switch (m_resizeDir)
    {
    case Left:
    case Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    default:
        //位于图标之后，最小化之前 检测是否在标题栏，设置拖动光标
        QRect titleBarRect(ui->label_system_icon->width(), 0, ui->btn_min->x() - ui->label_system_icon->width(), ui->label_system_icon->height());
        setCursor(titleBarRect.contains(currentPos) ? Qt::OpenHandCursor : Qt::ArrowCursor);
        break;
    }

    QWidget::mouseMoveEvent(event);
}

// 鼠标释放事件
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isDragging = false;
        m_isResizing = false;
        m_resizeDir = None;
        setCursor(Qt::ArrowCursor); // 恢复默认光标
    }
    QWidget::mouseReleaseEvent(event);
}

// 窗口大小改变事件（可选：用于处理缩放后的额外逻辑）
void MainWindow::resizeEvent(QResizeEvent *event)
{
    //调整上面的widget大小/因为左右两边都减小了，因此需要减少2个4
    QRect rectCentralwidget(4, 4, event->size().width() - 4 - 4, event->size().height() - 4 - 4);
    ui->centralwidget->setGeometry(rectCentralwidget);

    //调整上面的header区域(可以调整宽度，不能调整高度,因此不能如何缩放高度位置固定)
    QRect rectHeader(rectCentralwidget.x(), rectCentralwidget.y(), rectCentralwidget.width(), 24);
    ui->horizontal_layout_header->parentWidget()->setGeometry(rectHeader);

    //调整分割线1(可以调整宽度，不能调整高度,因此不能如何缩放高度位置固定)
    QRect rectSplit1 = ui->label_splite_1->geometry();
    rectSplit1.setX(rectCentralwidget.x());
    rectSplit1.setWidth(rectCentralwidget.width());
    ui->label_splite_1->setGeometry(rectSplit1);

    //tool工具栏(可以调整宽度，不能调整高度,因此不能如何缩放高度位置固定)
    QRect rectTools = ui->horizontal_layout_tool->parentWidget()->geometry();
    rectTools.setX(rectCentralwidget.x());
    rectTools.setWidth(rectCentralwidget.width());
    ui->horizontal_layout_tool->parentWidget()->setGeometry(rectTools);

     //调整分割线2(可以调整宽度，不能调整高度,因此不能如何缩放高度位置固定)
    QRect rectSplit2 = ui->label_splite_2->geometry();
    rectSplit2.setX(rectCentralwidget.x());
    rectSplit2.setWidth(rectCentralwidget.width());
    ui->label_splite_2->setGeometry(rectSplit2);

    //调整最下面的提示窗口(可以调整宽度，不能调整高度,因此不能如何缩放高度位置固定)
    QRect rectBottom(rectCentralwidget.x(), rectCentralwidget.height() - 24, rectCentralwidget.width(), 24);
    ui->label_bottom->setGeometry(rectBottom);

    //table的高度和宽度(只允许调整它的高度)
    QRect rectTable  = ui->horizontal_layout_table->parentWidget()->geometry();
    rectTable.setX(rectCentralwidget.x());
    rectTable.setWidth(rectCentralwidget.width());
    rectTable.setHeight(rectCentralwidget.height()
                        - rectHeader.height()
                        - rectSplit1.height()
                        - rectTools.height()
                        - rectSplit2.height()
                        - rectBottom.height());
    ui->horizontal_layout_table->parentWidget()->setGeometry(rectTable);
    QWidget::resizeEvent(event);
}

