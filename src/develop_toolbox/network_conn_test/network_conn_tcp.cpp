#include "network_conn_tcp.h"
#include "ui_network_conn_tcp.h"
#include "sched_mngt_message_box.h"
#include "sched_mngt_tcp.h"
#include "sched_mngt_utils.h"


/////////////////////////////////////////////////////////////////////////////////////
thread_tcp_connect::thread_tcp_connect(const QString &qstrHost,  int nPort, int nTimeout)
    : m_qstrHost(qstrHost)
    , m_nPort(nPort)
    , m_nTimeout(nTimeout)
{

}

//线程函数
void thread_tcp_connect::run() {
    //TCP连接
     CTcp tcp;
     tcp.InitSocket(m_nTimeout);
     //连接
     if (CONNECT_SUCCESS == tcp.ConnectServer(GetStdUtf8String(m_qstrHost).c_str(), m_nPort)) {
         emit OnConnectResultSignal(true);
     } else {
         emit OnConnectResultSignal(false);
     }
}

/////////////////////////////////////////////////////////////////////////////////////
network_conn_tcp::network_conn_tcp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::network_conn_tcp)
{
    ui->setupUi(this);
    //绑定
    connect(ui->pushButton, &QPushButton::clicked, this, &network_conn_tcp::StartConnect);
    connect(&m_timerProgressBar, &QTimer::timeout, this, &network_conn_tcp::OnTimerProgressBar);
    //默认超时时间5s
    ui->line_edit_time_out->setText(QStringLiteral("5000"));
    //设置进度条
    ui->progressBar->setValue(0);
}

network_conn_tcp::~network_conn_tcp()
{
    delete ui;
}

//开始连接测试
void network_conn_tcp::StartConnect() {
    //先禁止再次点击
    ui->pushButton->setEnabled(false);

    //获取参数
    QString qstrIP = ui->line_edit_ip->text();
    QString qstrPort = ui->line_edit_port->text();
    QString qstrTimeout = ui->line_edit_time_out->text();
    // 启动测试线程
    m_ptrThreadTcp.reset(new thread_tcp_connect(qstrIP, qstrPort.toInt(), qstrTimeout.toInt()));
    m_ptrThreadTcp->start();
    connect(m_ptrThreadTcp.get(), &thread_tcp_connect::OnConnectResultSignal, this, &network_conn_tcp::OnConnectResult);
    //开启线程
    m_timerProgressBar.start(1000);
}

//进度条更新
void network_conn_tcp::OnTimerProgressBar() {
    //得到时值
    QString qstrTimeout = ui->line_edit_time_out->text();
    int nTime = qstrTimeout.toUInt();
    int nValue = ui->progressBar->value();
    nValue += 100 / (nTime / 1000);
    //是否超限
    if (nValue > ui->progressBar->maximum()) {
        nValue = 0;
    }
    ui->progressBar->setValue(nValue);
}

//槽， 收到当前正在处理的文件
void network_conn_tcp::OnConnectResult(bool bSuc) {
    //连接结果
    if (bSuc) {
        sched_mngt_message_box::question(QStringLiteral("连接成功"), sched_mngt_message_box::MBT_INFO);
    } else {
        sched_mngt_message_box::question(QStringLiteral("连接失败"), sched_mngt_message_box::MBT_INFO);
    }
    //结束 进度条更新
    ui->progressBar->setValue(100);
    m_timerProgressBar.stop();
    m_ptrThreadTcp.reset();
    ui->pushButton->setEnabled(true);
}
