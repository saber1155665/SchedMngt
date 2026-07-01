#include "timed_task_mngt.h"
#include "ui_timed_task_mngt.h"
#include "timed_task.h"
#include "sqlite_tools.h"
#include "sched_mngt_message_box.h"

#include <QComboBox>
#include <QStringLiteral>

//每页数量
#define PAGE_TIMED_TASK_COUNT 10

//保存关键的KEY的项
#define COMBO_BOX_KEY_ID (char*)("ITEM_ID")

timed_task_mngt::timed_task_mngt(run_timed_task *pRunTimedTask, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::timed_task_mngt)
    , m_nPage(0)
    , m_nPageCount(0)
    , m_pRunTimedTask(pRunTimedTask)
{
    ui->setupUi(this);
    // 初始化选项操作
    m_qmapComboxBoxAction[QStringLiteral("")] = CBAT_SPACE;
    m_qmapComboxBoxAction[QStringLiteral("删除")] = CBAT_DELETE;
    m_qmapComboxBoxAction[QStringLiteral("修改")] = CBAT_CHANGE;

    //设置无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //关联槽
    connect(ui->btn_close, &QToolButton::clicked, this, &timed_task_mngt::Exit);
    connect(ui->btn_add_task_mgmt, &QToolButton::clicked, this, &timed_task_mngt::AddNewTimedTask);
    connect(ui->btn_next_page, &QToolButton::clicked, this, &timed_task_mngt::NextPage);
    connect(ui->btn_priv_page, &QToolButton::clicked, this, &timed_task_mngt::PrivPage);
    connect(ui->line_edit_page_no, &QLineEdit::returnPressed, this, &timed_task_mngt::OnChangePage);

    //设置表
    QStringList strListHeader;
    strListHeader << QStringLiteral("编号")
                  << QStringLiteral("ID")
                  << QStringLiteral("任务名")
                  << QStringLiteral("备注")
                  << QStringLiteral("运行方式")
                  << QStringLiteral("运行时间")
                  << QStringLiteral("添加时间")
                  << QStringLiteral("选择")
                  ;
    ui->tableWidget->setColumnCount(strListHeader.size()); //列数为
    ui->tableWidget->setHorizontalHeaderLabels(strListHeader);//设置header
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//列宽自适应
    ui->tableWidget->horizontalHeader()->setMinimumSectionSize(48);//设置列最小宽度
    //刷新数据
    RefreshTbData();
}

timed_task_mngt::~timed_task_mngt()
{
    delete ui;
}

//添加新的定时任务
void timed_task_mngt::AddNewTimedTask() {
    timed_task *p_timed_task = new timed_task(this);
    p_timed_task->setWindowModality(Qt::WindowModal);
    p_timed_task->show();
    connect(p_timed_task, SIGNAL(SendAddNewTimedTask()), this, SLOT(OnAddNewTimedTask()));
    //刷新数据
    RefreshTbData();
}
//退出窗口
void timed_task_mngt::Exit() {
    timed_task_mngt::close();
}

//刷新数据
void timed_task_mngt::RefreshTbData() {
    QList<TimedTaskItem> listTimedTask;
    //得到总数
    m_nPageCount = sqlite_tools::get_sqlite_tools().GetTimedTaskCount();
    m_nPageCount = (m_nPageCount + PAGE_TIMED_TASK_COUNT - 1) / PAGE_TIMED_TASK_COUNT;
    //查询数据库
    sqlite_tools::get_sqlite_tools().QueryTimedTask(m_nPage * PAGE_TIMED_TASK_COUNT, PAGE_TIMED_TASK_COUNT, listTimedTask);
    //清空
    ui->tableWidget->setRowCount(0);
    //得到类型对应名
    auto mapRunType = timed_task::InitRunTypeAndString();
    //添加到表
    int nRow = 0;
    for (auto iter = listTimedTask.begin(); iter != listTimedTask.end(); iter++) {
        int nRowCount = ui->tableWidget->rowCount();

        //
        auto pFuncNewLabe=[&](const QString &text) {
            QLabel *pLabel = new QLabel(text, this);
            pLabel->setContentsMargins(5, 5, 5, 5); // 设置上、右、下、左的边距
            pLabel->setMinimumWidth(48);
            pLabel->adjustSize();
            return pLabel;
        };
        int nColumn = 0;
        //添加一行
        ui->tableWidget->insertRow(nRowCount);
        //编号
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(QString("%1").arg(nRow)));
        //id
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(QString("%1").arg(iter->nId)));
        //任务名
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(iter->qstrName));
        //备注
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(iter->qstrNote));
        //运行方式
        for (auto iterRunType = mapRunType.begin(); iterRunType != mapRunType.end(); iterRunType++) {
            if (iterRunType.value() == iter->nRunType) {
                ui->tableWidget->setCellWidget(nRow, nColumn, pFuncNewLabe(iterRunType.key()));
            }
        }
        nColumn++;
        //运行时间
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(iter->qstrRunTime));
        //添加时间
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(iter->qstrCreateTime));
        //添加选项
        QStringList stringList;
        QComboBox *pComboBox = new QComboBox(this);
        for (auto iter = m_qmapComboxBoxAction.begin(); iter != m_qmapComboxBoxAction.end(); iter ++) {
            stringList << iter.key();
        }
        pComboBox->addItems(stringList);
        pComboBox->setProperty(COMBO_BOX_KEY_ID, iter->nId);
        pComboBox->setStyleSheet("QComboBox {background-color: transparent;}"
                                 "QComboBox::drop-down {background-color: transparent;}"
                                 );
        ui->tableWidget->setCellWidget(nRow, nColumn++, pComboBox);
        //关联槽,选择int参数的函数
        connect(pComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &timed_task_mngt::OnComboxBoxChange);

        nRow++;
    }
    //隐藏ID列.
    ui->tableWidget->setColumnHidden(1, true);

    //设置页数与总数
    int nTmpPage = m_nPage;
    if (0 ==  m_nPageCount) {//没有页时候，第一页肯定是0
        nTmpPage = 0;
    } else {
        nTmpPage += 1;
        //不能超过最大值
        if (nTmpPage > m_nPageCount) {
            nTmpPage = m_nPageCount;
        }
    }
    QString qstrPage = QString("%1").arg(nTmpPage);
    QString qstrPageCount = QString("/ %1").arg(m_nPageCount);
    ui->line_edit_page_no->setText(qstrPage);
    ui->label_page_count->setText(qstrPageCount);
}

//页面+1
void timed_task_mngt::NextPage() {
    m_nPage++;
    if (m_nPage >= m_nPageCount) {
        m_nPage = m_nPageCount - 1;
        sched_mngt_message_box::question(QStringLiteral("查询到了最后一页"), sched_mngt_message_box::MBT_INFO);
    }
    RefreshTbData();
}
//页面-1
void timed_task_mngt::PrivPage() {
    //页数-1
    m_nPage--;
    if (m_nPage <= 0) {
        m_nPage = 0;
        sched_mngt_message_box::question(QStringLiteral("查询到了第一页"), sched_mngt_message_box::MBT_INFO);
    }
    //刷新数据
    RefreshTbData();
}

//手动在页面数那里修改了数字
void timed_task_mngt::OnChangePage()  {
    QString qstrPageNo = ui->line_edit_page_no->text();
    int nTmpPageNo = qstrPageNo.toInt();
    if (nTmpPageNo < 0 || nTmpPageNo > m_nPageCount) {
        sched_mngt_message_box::question(QStringLiteral("错误，输入值不能超过范围"), sched_mngt_message_box::MBT_ERROR);
        return;
    }
    m_nPage = nTmpPageNo - 1;
    RefreshTbData();
}

//键盘按下消息
void timed_task_mngt::keyPressEvent(QKeyEvent *event) {
    if (Qt::Key_Return == event->key()) {
        event->setAccepted(false);
    }
}


//编辑或者删除的combox选项触发消息
void timed_task_mngt::OnComboxBoxChange(int index) {
    QComboBox *pQComboBox = static_cast<QComboBox *>(QObject::sender());
    //数据的KEY ID
    auto keyID = pQComboBox->property(COMBO_BOX_KEY_ID);
    int nId = keyID.toInt();
    //得到操作项
    QString qstrText = pQComboBox->currentText();
    ComboxBoxActionType nAction = m_qmapComboxBoxAction[qstrText];
    if (CBAT_DELETE == nAction) {//删除项
        if (sqlite_tools::get_sqlite_tools().DeleteTimedTask(nId)) {
            if (m_pRunTimedTask) {
                m_pRunTimedTask->RefreshTaskList();
            }
            sched_mngt_message_box::question(QStringLiteral("删除成功，即将刷新数据"), sched_mngt_message_box::MBT_INFO);
            //刷新数据
            RefreshTbData();
        }
    }
}


//信号槽，添加了新的定时任务
void timed_task_mngt::OnAddNewTimedTask() {
    //刷新数据
    RefreshTbData();
    //更新提醒
    if (m_pRunTimedTask) {
        m_pRunTimedTask->RefreshTaskList();
    }
}
