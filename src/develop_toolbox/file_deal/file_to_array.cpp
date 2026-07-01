#include "file_to_array.h"
#include "ui_file_to_array.h"
#include "self_define_enc_utils.h"
#include "sched_mngt_message_box.h"
#include "sched_mngt_utils.h"

#include <QRandomGenerator>
#include <QFile>


file_to_array::file_to_array(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::file_to_array)
{
    ui->setupUi(this);

    //绑定
    connect(ui->push_button_start, &QPushButton::clicked, this, &file_to_array::Start);

    //算法类型与算法名映射
    m_mapAlgorithmName[QStringLiteral("不加密")] =FEA_NO_ENCRYPT;
    m_mapAlgorithmName[QStringLiteral("随机选择算法")] =FEA_Random;
    m_mapAlgorithmName[QStringLiteral("随机移位亦或算法")] =FEA_ShitfOxr;
    m_mapAlgorithmName[QStringLiteral("分解成商与余数算法")] =FEA_Mod;
    QStringList stringListAlgorithm;
    for (auto iter = m_mapAlgorithmName.begin(); iter != m_mapAlgorithmName.end(); iter++) {
        stringListAlgorithm << iter.key();
    }
    ui->combo_box_encrypt_algorithm->addItems(stringListAlgorithm);
    for (auto iter = m_mapAlgorithmName.begin(); iter != m_mapAlgorithmName.end(); iter++) {
        if (FEA_NO_ENCRYPT == iter.value()) {
            ui->combo_box_encrypt_algorithm->setCurrentText(iter.key());
        }
    }
}

file_to_array::~file_to_array()
{
    delete ui;
}

//开始转
void file_to_array::Start() {
    //得到
    QString qstrSrcFile = ui->line_edit_src_file->text();
    QString qstrDstFile = ui->line_edit_dst_file->text();
    QString qstrParamName = ui->line_edit_param_name->text();

    //根据算法选择不同
    FileEncryptAlgorithm  nEncryptAlgo = m_mapAlgorithmName[ui->combo_box_encrypt_algorithm->currentText()];
    if (FEA_Random == nEncryptAlgo) {//随机算法的话，就做一次随机
        quint32 u32Rand = QRandomGenerator::global()->bounded(quint32(FEA_Count - FEA_Random - 1));
        nEncryptAlgo = static_cast<FileEncryptAlgorithm>(u32Rand + FEA_ShitfOxr);
    }

    //读取文件
    QFile file(qstrSrcFile);
    // 以只读、二进制模式打开（兼容文本/二进制文件）
    if (!file.open(QIODevice::ReadOnly)) {
        sched_mngt_message_box::question(QStringLiteral("打开文件失败"), sched_mngt_message_box::MBT_ERROR);
        return ;
    }

    // 读取文件全部内容
    QByteArray content = file.readAll();
    file.close();

    //加密
    QString qstrEncData;
    switch (nEncryptAlgo) {
    case FEA_ShitfOxr: {
         qstrEncData = self_define_enc_utils::BinaryRandomShiftOxr(content, qstrParamName);
    }
        break;
    case FEA_Mod: {
        qstrEncData = self_define_enc_utils::BinaryRandomMod(content, qstrParamName);
    }
         break;
    case FEA_NO_ENCRYPT: {
        qstrEncData = self_define_enc_utils::Bin2Hex(content, 4);
    }
        break;
    default: {
        sched_mngt_message_box::question(QStringLiteral("错误加密算法"), sched_mngt_message_box::MBT_INFO);
    }
        break;
    }

    WriteData2File(qstrDstFile, qstrEncData.toUtf8());
    sched_mngt_message_box::question(QStringLiteral("转数组成功"), sched_mngt_message_box::MBT_INFO);
}
