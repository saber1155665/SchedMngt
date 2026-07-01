#ifndef NETWORK_CONN_TEST_H
#define NETWORK_CONN_TEST_H

#include "fast_start_key_value.h"

#include <QWidget>
#include <QMap>

namespace Ui {
class network_conn_test;
}

class network_conn_test : public QWidget
{
    Q_OBJECT

public:
    explicit network_conn_test(QWidget *parent = nullptr);
    ~network_conn_test();

    //快捷输出的影响
    void ChangeItem(const TreeItemAction nAction);
private:
    void ComboxChangeItem(const QString & str);
    void showEvent(QShowEvent *event);
private:
    Ui::network_conn_test *ui;
    //
    QMap<QString, QWidget*> m_mapWidget;
};

#endif // NETWORK_CONN_TEST_H
