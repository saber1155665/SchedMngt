#ifndef NETWORK_CONN_TCP_H
#define NETWORK_CONN_TCP_H

#include <QWidget>
#include <QTimer>
#include <QThread>

namespace Ui {
class network_conn_tcp;
}


class thread_tcp_connect : public QThread {
    Q_OBJECT
public:
    thread_tcp_connect(const QString &qstrHost,  int nPort, int nTimeout);
    virtual ~thread_tcp_connect(){};

    void run() override;
signals:
   //槽， 收到当前正在处理的文件
    void OnConnectResultSignal(bool bSuc);

private:
    QString m_qstrHost;
    int m_nPort;
    int m_nTimeout;
};

class network_conn_tcp : public QWidget
{
    Q_OBJECT

public:
    explicit network_conn_tcp(QWidget *parent = nullptr);
    ~network_conn_tcp();

private slots:
    //槽， 收到当前正在处理的文件
    void OnConnectResult(bool bSuc);
private:
    //开始连接测试
    void StartConnect();
    //进度条更新
    void OnTimerProgressBar();
private:
    Ui::network_conn_tcp *ui;
    //更新进度条
    QTimer m_timerProgressBar;
    //
    std::shared_ptr<thread_tcp_connect> m_ptrThreadTcp;
};

#endif // NETWORK_CONN_TCP_H
