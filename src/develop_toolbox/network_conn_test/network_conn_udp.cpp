#include "network_conn_udp.h"
#include "ui_network_conn_udp.h"

network_conn_udp::network_conn_udp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::network_conn_udp)
{
    ui->setupUi(this);
}

network_conn_udp::~network_conn_udp()
{
    delete ui;
}
