#ifndef NETWORK_CONN_ICMP_H
#define NETWORK_CONN_ICMP_H

#include <QWidget>

namespace Ui {
class network_conn_icmp;
}

class network_conn_icmp : public QWidget
{
    Q_OBJECT

public:
    explicit network_conn_icmp(QWidget *parent = nullptr);
    ~network_conn_icmp();

private:
    Ui::network_conn_icmp *ui;
};

#endif // NETWORK_CONN_ICMP_H
