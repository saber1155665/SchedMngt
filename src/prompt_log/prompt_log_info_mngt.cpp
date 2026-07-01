#include "prompt_log_info_mngt.h"
#include "ui_prompt_log_info_mngt.h"
#include "timed_task.h"
#include "sqlite_tools.h"
#include "sched_mngt_message_box.h"

#include <QComboBox>
#include <QStringLiteral>

//每页数量
#define PAGE_TIMED_TASK_COUNT 10

//保存关键的KEY的项
#define COMBO_BOX_KEY_ID (char*)("ITEM_ID")

prompt_log_info_mngt::prompt_log_info_mngt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::prompt_log_info_mngt)
    , m_nPage(0)
    , m_nPageCount(0)
{
    ui->setupUi(this);
    //设置无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //关联槽
    connect(ui->btn_close, &QToolButton::clicked, this, &prompt_log_info_mngt::Exit);
    connect(ui->btn_next_page, &QToolButton::clicked, this, &prompt_log_info_mngt::NextPage);
    connect(ui->btn_priv_page, &QToolButton::clicked, this, &prompt_log_info_mngt::PrivPage);
    connect(ui->line_edit_page_no, &QLineEdit::returnPressed, this, &prompt_log_info_mngt::OnChangePage);

    //设置表
    QStringList strListHeader;
    strListHeader << QStringLiteral("编号")
                  << QStringLiteral("ID")
                  << QStringLiteral("创建时间")
                  << QStringLiteral("日志内容")
                  ;
    ui->tableWidget->setColumnCount(strListHeader.size()); //列数为
    ui->tableWidget->setHorizontalHeaderLabels(strListHeader);//设置header
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//列宽自适应
    ui->tableWidget->horizontalHeader()->setMinimumSectionSize(48);//设置列最小宽度
    //刷新数据
    RefreshTbData();
}

prompt_log_info_mngt::~prompt_log_info_mngt()
{
    delete ui;
}

//退出窗口
void prompt_log_info_mngt::Exit() {
    prompt_log_info_mngt::close();
}

//刷新数据
void prompt_log_info_mngt::RefreshTbData() {
    QList<PromptLogItem> listPromptLog;
    //得到总数
    m_nPageCount = sqlite_tools::get_sqlite_tools().GetPromptLogCount();
    m_nPageCount = (m_nPageCount + PAGE_TIMED_TASK_COUNT - 1) / PAGE_TIMED_TASK_COUNT;
    //查询数据库
    sqlite_tools::get_sqlite_tools().QueryPromptLog(m_nPage * PAGE_TIMED_TASK_COUNT, PAGE_TIMED_TASK_COUNT, listPromptLog);
    //清空
    ui->tableWidget->setRowCount(0);
    //得到类型对应名
    auto mapRunType = timed_task::InitRunTypeAndString();
    //添加到表
    int nRow = 0;
    for (auto iter = listPromptLog.begin(); iter != listPromptLog.end(); iter++) {
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
        //创建时间
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(iter->qstrCreateTime));
        //日志内容
        ui->tableWidget->setCellWidget(nRow, nColumn++, pFuncNewLabe(iter->qstrLogInfo));

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
void prompt_log_info_mngt::NextPage() {
    m_nPage++;
    if (m_nPage >= m_nPageCount) {
        m_nPage = m_nPageCount - 1;
        sched_mngt_message_box::question(QStringLiteral("查询到了最后一页"), sched_mngt_message_box::MBT_INFO);
    }
    RefreshTbData();
}
//页面-1
void prompt_log_info_mngt::PrivPage() {
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
void prompt_log_info_mngt::OnChangePage()  {
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
void prompt_log_info_mngt::keyPressEvent(QKeyEvent *event) {
    if (Qt::Key_Return == event->key()) {
        event->setAccepted(false);
    }
}

