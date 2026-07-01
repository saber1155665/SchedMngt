#include "network_conn_test.h"
#include "ui_network_conn_test.h"
#include "network_conn_icmp.h"
#include "network_conn_tcp.h"
#include "network_conn_udp.h"

#include <QStringLiteral>

network_conn_test::network_conn_test(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::network_conn_test)
{
    ui->setupUi(this);

    //绑定
    connect(ui->comboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), this, &network_conn_test::ComboxChangeItem);

    //初始化类型
    m_mapWidget[QStringLiteral("TCP")] = new network_conn_tcp(this);
    m_mapWidget[QStringLiteral("UDP")] = new network_conn_udp(this);
    m_mapWidget[QStringLiteral("ICMP")] = new network_conn_icmp(this);

    //初始化combox
    QStringList listStrType;
    for (auto iter = m_mapWidget.begin(); iter != m_mapWidget.end(); iter++) {
        listStrType.push_back(iter.key());
        ui->stackedWidget->addWidget(iter.value());
    }
    ui->comboBox->addItems(listStrType);

    ui->comboBox->setCurrentText(QStringLiteral("TCP"));
}

network_conn_test::~network_conn_test()
{
    delete ui;
}


void network_conn_test::ComboxChangeItem(const QString & str) {
    for (auto iter = m_mapWidget.begin(); iter != m_mapWidget.end(); iter++) {
        if (iter.key() == str) {
            ui->stackedWidget->setCurrentWidget(iter.value());
        }
    }
}

void network_conn_test::showEvent(QShowEvent *event) {
    return QWidget::showEvent(event);
}


//快捷输出的影响
void network_conn_test::ChangeItem(const TreeItemAction nAction) {
    if (TIA_NETWORK_CONNECT_TEST_TCP == nAction) {
        ui->comboBox->setCurrentText(QStringLiteral("TCP"));
    } else if (TIA_NETWORK_CONNECT_TEST_UDP == nAction){
        ui->comboBox->setCurrentText(QStringLiteral("UDP"));
    } else if (TIA_NETWORK_CONNECT_TEST_ICMP == nAction){
        ui->comboBox->setCurrentText(QStringLiteral("ICMP"));
    }
}
