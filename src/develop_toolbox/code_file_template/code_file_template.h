#ifndef CODE_FILE_TEMPLATE_H
#define CODE_FILE_TEMPLATE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class code_file_template;
}

class code_file_template : public QWidget
{
    Q_OBJECT

public:
    explicit code_file_template(QWidget *parent = nullptr);
    ~code_file_template();

    enum LanguageType {
        LT_CPP, //C++语言
        LT_C,   //C语言


        CT_COUNT
    };

private:
    //生成文件
    void Generate();

    //生成CPP文件
    void GenerateCPP();

    //生成C语言文件
    void GenerateC();

    //输入响应，生成文件名
    void GenereateName();

    //选择文件路径
    void ChooseDir();

    //选择语言的相应
    void SelectLanguage(const QString &);
private:
    Ui::code_file_template *ui;
    //语言和类型的影射
    QMap<QString, LanguageType> m_map;
};

#endif // CODE_FILE_TEMPLATE_H
