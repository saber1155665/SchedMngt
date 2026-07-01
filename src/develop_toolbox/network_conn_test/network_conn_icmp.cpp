#include "network_conn_icmp.h"
#include "ui_network_conn_icmp.h"

network_conn_icmp::network_conn_icmp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::network_conn_icmp)
{
    ui->setupUi(this);
}

network_conn_icmp::~network_conn_icmp()
{
    delete ui;
}
