#ifndef FORMAT_JSON_2_SHOW_H
#define FORMAT_JSON_2_SHOW_H

#include <QWidget>

namespace Ui {
class format_json_2_show;
}

class format_json_2_show : public QWidget
{
    Q_OBJECT

public:
    explicit format_json_2_show(QWidget *parent = nullptr);
    ~format_json_2_show();

    //开始格式化
    void Format();

private:
    Ui::format_json_2_show *ui;
};

#endif // FORMAT_JSON_2_SHOW_H
