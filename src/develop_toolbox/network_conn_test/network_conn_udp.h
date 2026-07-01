#ifndef NETWORK_CONN_UDP_H
#define NETWORK_CONN_UDP_H

#include <QWidget>

namespace Ui {
class network_conn_udp;
}

class network_conn_udp : public QWidget
{
    Q_OBJECT

public:
    explicit network_conn_udp(QWidget *parent = nullptr);
    ~network_conn_udp();

private:
    Ui::network_conn_udp *ui;
};

#endif // NETWORK_CONN_UDP_H
