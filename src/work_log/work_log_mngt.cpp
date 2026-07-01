#include "work_log_mngt.h"
#include "ui_work_log_mngt.h"
#include "work_log.h"
#include "sqlite_tools.h"
#include "sched_mngt_message_box.h"

#include <QTextEdit>

//每页数量
#define PAGE_WORK_LOG_COUNT 10


work_log_mngt::work_log_mngt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::work_log_mngt)
    , m_nPageCount(0)
    , m_nPage(0)
{
    ui->setupUi(this);
    //设置无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //关联
    connect(ui->btn_add, &QPushButton::clicked, this, &work_log_mngt::Add);
    connect(ui->btn_cacel, &QPushButton::clicked, this, &work_log_mngt::Exit);
    connect(ui->btn_close, &QPushButton::clicked, this, &work_log_mngt::Exit);
    connect(ui->btn_next_page, &QPushButton::clicked, this, &work_log_mngt::NextPage);
    connect(ui->btn_priv_page, &QPushButton::clicked, this, &work_log_mngt::PrivPage);
    //设置表
    QStringList strListHeader;
    strListHeader << QStringLiteral("编号")
                  << QStringLiteral("ID")
                  << QStringLiteral("日期")
                  << QStringLiteral("工作日志")
                  ;
    ui->tableWidget->setColumnCount(strListHeader.size()); //列数为
    ui->tableWidget->setHorizontalHeaderLabels(strListHeader);//设置header
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//列宽自适应
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 行高自适应内容（核心！）
    ui->tableWidget->horizontalHeader()->setMinimumSectionSize(48);//设置列最小宽度
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);//最后一列日志内容填满表格
    //刷新数据
    RefreshTbData();
}

work_log_mngt::~work_log_mngt()
{
    delete ui;
}

//刷新数据
void work_log_mngt::RefreshTbData()  {
    QList<WorkLogItem> listLogItem;
    //得到总数
    m_nPageCount = sqlite_tools::get_sqlite_tools().GetWorkLogCount();
    m_nPageCount = (m_nPageCount + PAGE_WORK_LOG_COUNT - 1) / PAGE_WORK_LOG_COUNT;
    //查询数据库
    sqlite_tools::get_sqlite_tools().QueryWorkLog(m_nPage * PAGE_WORK_LOG_COUNT, PAGE_WORK_LOG_COUNT, listLogItem);
    //清空
    ui->tableWidget->setRowCount(0);

    //添加到表
    int nRow = 0;
    for (auto iter = listLogItem.begin(); iter != listLogItem.end(); iter++) {
        int nRowCount = ui->tableWidget->rowCount();
        //添加一行
        ui->tableWidget->insertRow(nRowCount);

        int nColumn = 0;
        //
        auto pFuncNewLabe=[&](const QString &text) {
            QLabel *pLabel = new QLabel(text, this);
            pLabel->setContentsMargins(5, 5, 5, 5); // 设置上、右、下、左的边距
            pLabel->setMinimumWidth(48);
            pLabel->adjustSize();
            return pLabel;
        };
        //编号
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(QString("%1").arg(nRow)));
        //id
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(QString("%1").arg(iter->nId)));
        //时间
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(iter->qstrCreateTime));
        //内容
        auto pFuncNewEdit = [&](QString &text) {
            //text.replace("\n", "\r\n");
            QTextEdit *pTextEdit = new QTextEdit(this);
            pTextEdit->setContentsMargins(5, 5, 5, 5); // 设置上、右、下、左的边距
            pTextEdit->setMinimumWidth(48);
            pTextEdit->adjustSize();
            pTextEdit->setReadOnly(true);
            pTextEdit->setWordWrapMode(QTextOption::WordWrap);
            pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条（避免换行失效）
            pTextEdit->setPlainText(text);
            return pTextEdit;
        };
        //工作日志
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewEdit(iter->qstrNote));

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


//退出窗口
void work_log_mngt::Exit() {
    work_log_mngt::close();
}

//添加项
void work_log_mngt::Add() {
    work_log *p_work_log = new work_log(this);
    p_work_log->setWindowModality(Qt::WindowModal);
    p_work_log->show();
    connect(p_work_log, SIGNAL(SendAddNewWorkLog()), this, SLOT(OnAddNewWorkLog()));
}

//页面+1
void work_log_mngt::NextPage() {
    m_nPage++;
    if (m_nPage >= m_nPageCount) {
        m_nPage = m_nPageCount - 1;
        sched_mngt_message_box::question(QStringLiteral("查询到了最后一页"), sched_mngt_message_box::MBT_INFO);
    }
    RefreshTbData();
}
//页面-1
void work_log_mngt::PrivPage() {
    //页数-1
    m_nPage--;
    if (m_nPage <= 0) {
        m_nPage = 0;
        sched_mngt_message_box::question(QStringLiteral("查询到了第一页"), sched_mngt_message_box::MBT_INFO);
    }
    //刷新数据
    RefreshTbData();
}


//信号槽，添加了新的定时任务
void work_log_mngt::OnAddNewWorkLog() {
    //刷新数据
    RefreshTbData();
}
