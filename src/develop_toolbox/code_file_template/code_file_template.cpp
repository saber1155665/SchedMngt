#include "code_file_template.h"
#include "ui_code_file_template.h"
#include "sched_mngt_utils.h"

#include <QFileDialog>
#include <QDebug>
#include <QUuid>

code_file_template::code_file_template(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::code_file_template)
{
    ui->setupUi(this);

    //绑定
    connect(ui->btn_generate, &QPushButton::clicked, this, &code_file_template::Generate);
    connect(ui->btn_choose_dir, &QPushButton::clicked, this, &code_file_template::ChooseDir);
    connect(ui->line_edit_obj, &QLineEdit::textChanged, this, &code_file_template::GenereateName);
    connect(ui->combo_box_language, QOverload<const QString &>::of(&QComboBox::currentTextChanged), this, &code_file_template::SelectLanguage);

    m_map[QStringLiteral("C++语言")] = LT_CPP;
    m_map[QStringLiteral("C语言")] = LT_C;
    QStringList qstrListLanguage;
    for (auto iter = m_map.begin(); iter != m_map.end(); iter++) {
        qstrListLanguage.push_back(iter.key());
    }
    ui->combo_box_language->addItems(qstrListLanguage);
}

code_file_template::~code_file_template()
{
    delete ui;
}


//生成文件
void code_file_template::Generate() {
    QString qstrLanguage = ui->combo_box_language->currentText();

    auto iterFind = m_map.find(qstrLanguage);
    if (m_map.end() == iterFind) {
        return;
    }
    switch (iterFind.value()) {
    case LT_CPP:
        GenerateCPP();
        break;

    case LT_C:
        GenerateC();
        break;

    default:
        break;
    }
}


//生成文件
void code_file_template::GenerateCPP() {
    //头文件处理
    //cpp模板
    QString qstrCppHeaderTemplate = QStringLiteral(
        "#pragma once\r\n"
        "\r\n"
        "//class $CLASS_NAME$\r\n"
        "class $CLASS_NAME$ {\r\n"
        "	public:\r\n"
        "	$CLASS_NAME$ ();\r\n"
        "	virtual ~$CLASS_NAME$ ();\r\n"
        "	\r\n"
        "	private:\r\n"
        "};\r\n"
        "\r\n"
     );
    QString qstrClassName = ui->line_edit_class_name->text();
    //替换类名
    QString qstrCppHeaderInfo = qstrCppHeaderTemplate.replace(QStringLiteral("$CLASS_NAME$"), qstrClassName);
    //
    QString qstrHeaderFilePath =  ui->line_edit_dest_dir->text();
    qstrHeaderFilePath = qstrHeaderFilePath.replace(QStringLiteral("\\"), QStringLiteral("/"));
    if (!qstrHeaderFilePath.endsWith("/")) {
        qstrHeaderFilePath += QStringLiteral("/");
    }
    qstrHeaderFilePath += ui->line_edit_header_file->text();
    WriteData2File(qstrHeaderFilePath, qstrCppHeaderInfo.toUtf8());

    //源代码文件处理
    QString qstrSourceTemplate =QStringLiteral("#include \"$HEADER_FILE_NAME$\" \r\n"
        "\r\n"
        "$CLASS_NAME$::$CLASS_NAME$() {\r\n"
        "}\r\n"
        "\r\n"
        "$CLASS_NAME$::~$CLASS_NAME$() {\r\n"
        "}\r\n"
        "\r\n"
    );
    QString qstrCppSourceInfo = qstrSourceTemplate.replace(QStringLiteral("$CLASS_NAME$"), qstrClassName);
    qstrCppSourceInfo = qstrCppSourceInfo.replace(QStringLiteral("$HEADER_FILE_NAME$"), ui->line_edit_header_file->text());
    //
    QString qstrSourceFilePath =  ui->line_edit_dest_dir->text();
    qstrSourceFilePath = qstrSourceFilePath.replace(QStringLiteral("\\"), QStringLiteral("/"));
    if (!qstrSourceFilePath.endsWith("/")) {
        qstrSourceFilePath += QStringLiteral("/");
    }
    qstrSourceFilePath += ui->line_edit_source_file->text();
    WriteData2File(qstrSourceFilePath, qstrCppSourceInfo.toUtf8());
}


//生成C语言文件
void code_file_template::GenerateC() {
    //头文件处理
    QString qstrHeaderFileName = ui->line_edit_header_file->text();
    // 生成随机UUID/GUID
    QString qstrGuid = QUuid::createUuid().toString();
    qstrGuid = qstrGuid.replace(QStringLiteral("{"), QStringLiteral(""));
    qstrGuid = qstrGuid.replace(QStringLiteral("}"), QStringLiteral(""));
    qstrGuid = qstrGuid.replace(QStringLiteral("-"), QStringLiteral("_"));
    qstrGuid = qstrGuid + QStringLiteral("_H_");
    //cpp模板
    QString qstrCppHeaderTemplate = QStringLiteral(
        "#ifndef $GUID_DEFINED$\r\n"
        "#defined $GUID_DEFINED$\r\n"
        "    \r\n"
        "\r\n"
        "#endif"
     );
    //文件GUID
    QString qstrCppHeaderInfo = qstrCppHeaderTemplate.replace(QStringLiteral("$GUID_DEFINED$"), qstrGuid);
    //
    QString qstrHeaderFilePath =  ui->line_edit_dest_dir->text();
    qstrHeaderFilePath = qstrHeaderFilePath.replace(QStringLiteral("\\"), QStringLiteral("/"));
    if (!qstrHeaderFilePath.endsWith("/")) {
        qstrHeaderFilePath += QStringLiteral("/");
    }
    qstrHeaderFilePath += qstrHeaderFileName;
    WriteData2File(qstrHeaderFilePath, qstrCppHeaderInfo.toUtf8());

    //源代码文件处理
    QString qstrSourceTemplate =QStringLiteral("#include \"$HEADER_FILE_NAME$\" \r\n"
        "\r\n"
        "\r\n"
    );
     QString qstrCppSourceInfo = qstrSourceTemplate.replace(QStringLiteral("$HEADER_FILE_NAME$"), ui->line_edit_header_file->text());
    //
    QString qstrSourceFilePath =  ui->line_edit_dest_dir->text();
    qstrSourceFilePath = qstrSourceFilePath.replace(QStringLiteral("\\"), QStringLiteral("/"));
    if (!qstrSourceFilePath.endsWith("/")) {
        qstrSourceFilePath += QStringLiteral("/");
    }
    qstrSourceFilePath += ui->line_edit_source_file->text();
    WriteData2File(qstrSourceFilePath, qstrCppSourceInfo.toUtf8());
}


//输入响应，生成文件名
void code_file_template::GenereateName() {
    //获取数据
    QString qstrName = ui->line_edit_obj->text();
    //转成目标文件名。小写下划线分割
    QString qstrLowName = qstrName.toLower();
    QString qstrResult = qstrLowName.replace(QStringLiteral(" "), QStringLiteral("_"));
    //类用驼峰命令
    QString qstrClassName = qstrName.replace(QStringLiteral(" "), QStringLiteral(""));
    //文件名用小写+下划线
    QString qstrHeaderFileName = qstrResult + QStringLiteral(".h");

    QString qstrLanguage = ui->combo_box_language->currentText();
    QString qstrSourceFileName = qstrResult + QStringLiteral(".cpp");
    auto iterFind = m_map.find(qstrLanguage);
    if (m_map.end() != iterFind) {
        if (LT_C ==  iterFind.value()) {
            qstrSourceFileName = qstrResult + QStringLiteral(".c");
        }
    }

    //设置显示
    ui->line_edit_header_file->setText(qstrHeaderFileName);
    ui->line_edit_source_file->setText(qstrSourceFileName);
    ui->line_edit_class_name->setText(qstrClassName);
}

//选择文件路径
void code_file_template::ChooseDir() {
    // 弹出目录选择窗口
    // 参数说明：
    // 1. 父窗口：&window（弹窗会依附于主窗口）
    // 2. 窗口标题："选择目标目录"
    // 3. 默认打开的目录："."（当前程序运行目录，也可指定绝对路径如"C:/"）
    // 4. 选项：ShowDirsOnly 表示只显示目录，DontResolveSymlinks 不解析符号链接
    QString selectedDir = QFileDialog::getExistingDirectory(
        this,
        QStringLiteral("选择目标目录"),
        QStringLiteral("."),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    // 判断用户是否选择了目录（点击取消则返回空字符串）
    if (!selectedDir.isEmpty()) {
         ui->line_edit_dest_dir->setText(selectedDir);
         ui->line_edit_dest_dir->setToolTip(selectedDir);
        qDebug() << "用户选择的目录路径：" << selectedDir;
    } else {
        ui->line_edit_dest_dir->setText(QString());
        ui->line_edit_dest_dir->setToolTip(QString());
        qDebug() << "用户取消了目录选择";
    }
}

//选择语言的相应
void code_file_template::SelectLanguage(const QString &qstrSelect) {
    QString qstrLanguage = ui->combo_box_language->currentText();

    auto iterFind = m_map.find(qstrLanguage);
    if (m_map.end() == iterFind) {
        return;
    }
    switch (iterFind.value()) {
    case LT_CPP:{
         for (int i = 0; i < ui->horizontal_layout_class_name_2->count(); ++i) {
             QWidget* w = ui->horizontal_layout_class_name_2->itemAt(i)->widget();
             if (w != nullptr) {
                 w->show();
             }
         }
    }
        break;

    case LT_C:{
        for (int i = 0; i < ui->horizontal_layout_class_name_2->count(); ++i) {
            QWidget* w = ui->horizontal_layout_class_name_2->itemAt(i)->widget();
            if (w != nullptr) {
                w->hide();
            }
        }
    }
        break;

    default:
        break;
    }
}
