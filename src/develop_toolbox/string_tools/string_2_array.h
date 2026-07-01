#ifndef STRING_2_ARRAY_H
#define STRING_2_ARRAY_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class string_2_array;
}

class string_2_array : public QWidget
{
    Q_OBJECT

public:
    explicit string_2_array(QWidget *parent = nullptr);
    ~string_2_array();

private:
    //转换成数组
    void OnChange2Arrary();

private:
    //
    Ui::string_2_array *ui;
    //定时检测，自动更新数组
    QTimer m_timerChange2Array;
    //记录上一次内容
    QString m_qstrPriv;
};

#endif // STRING_2_ARRAY_H
