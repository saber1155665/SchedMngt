#include "string_2_array.h"
#include "ui_string_2_array.h"

#define TIMER_INTERVAL_CHANGE 1000

string_2_array::string_2_array(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::string_2_array)
{
    ui->setupUi(this);
    //绑定
    connect(ui->btn_change, &QPushButton::clicked, this, &string_2_array::OnChange2Arrary);
    connect(ui->text_edit_src, &QTextEdit::textChanged, this, &string_2_array::OnChange2Arrary);
    //每一秒触发一次
    //connect(&m_timerChange2Array, &QTimer::timeout, this, &string_2_array::OnChange2Arrary);
    //m_timerChange2Array.start(TIMER_INTERVAL_CHANGE); // 开启
}

string_2_array::~string_2_array()
{
    delete ui;
}

//转换成数组
void string_2_array::OnChange2Arrary() {
    //获取
    QString qstrSrc = ui->text_edit_src->toPlainText();
    //为空没必要更新
    if (qstrSrc.isEmpty()) {
        return;
    }
    //和上次一样也不更新
    if (qstrSrc == m_qstrPriv) {
        return;
    }
    //设置注释
    QString qstrComment = "//" + qstrSrc + "\r\n";
    //转字符串
    QString qstrArray;
    //类型
    qstrArray += "char xxxx[] = { ";
    for (int i = 0; i < qstrSrc.size(); i++) {
        if ((0 != i) && (0 == (i % 16))) {
            qstrArray += "\r\n                ";
        }
        //换行
        qstrArray += "\'" + qstrSrc[i] + "\', ";
    }
    //末尾的\0
    qstrArray += "\'\\0\'};";
    //设置值
    QString qstrInfo = qstrComment + qstrArray;
    ui->text_edit_dst->setText(qstrInfo);
    //
    m_qstrPriv = qstrSrc;
}
