#include "format_json_2_show.h"
#include "ui_format_json_2_show.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

format_json_2_show::format_json_2_show(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::format_json_2_show)
{
    ui->setupUi(this);

    //绑定
    connect(ui->push_button_format, &QPushButton::clicked, this, &format_json_2_show::Format);

}

format_json_2_show::~format_json_2_show()
{
    delete ui;
}


//开始格式化
void format_json_2_show::Format() {
    QString qstrSrc = ui->text_edit_src->toPlainText();

    // 1. 解析原始JSON字符串为QJsonDocument
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(qstrSrc.toUtf8(), &parseError);

    // 2. 检查解析是否成功
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << QStringLiteral("JSON解析失败：") << parseError.errorString();
        return;
    }

    // 3. 格式化输出（Indented），对比：Compact是紧凑格式
    QByteArray formattedJson = jsonDoc.toJson(QJsonDocument::Indented);

    // 4. 转换为QString并返回
    ui->text_edit_dst->setPlainText(QString(formattedJson));
}
