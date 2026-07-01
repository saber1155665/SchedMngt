#include "string_random_dec.h"
#include "ui_string_random_dec.h"
#include "sched_mngt_utils.h"#
#include  "sched_mngt_message_box.h"
#include "self_define_enc_utils.h"

#include <QRandomGenerator>
#include <QVector>
#include <QDebug>

#define TIMER_INTERVAL_CHANGE 1000

string_random_dec::string_random_dec(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::string_random_dec)
{
    ui->setupUi(this);
    //绑定
    connect(ui->btn_change, &QPushButton::clicked, this, &string_random_dec::OnStartEncrypt);
    connect(ui->text_edit_src, &QTextEdit::textChanged, this, &string_random_dec::OnStartEncrypt);
    connect(ui->line_edit_param_name, &QLineEdit::textChanged, this, &string_random_dec::OnStartEncrypt);
    connect(ui->combo_box_param_type,QOverload<const QString &>::of(&QComboBox::currentIndexChanged), this, &string_random_dec::ParamTypeChanged);
    connect(ui->combo_box_encrypt_algorithm,QOverload<const QString &>::of(&QComboBox::currentIndexChanged), this, &string_random_dec::AlgorithmChanged);

    //设置单位
    m_mapParamType[QStringLiteral("uint8_t")] = PT_UINT8;
    m_mapParamType[QStringLiteral("uint16_t")] = PT_UINT16;
    QStringList stringListUnit;
    for (auto iter = m_mapParamType.begin(); iter != m_mapParamType.end(); iter++) {
        stringListUnit << iter.key();
    }
    ui->combo_box_param_type->addItems(stringListUnit);
    for (auto iter = m_mapParamType.begin(); iter != m_mapParamType.end(); iter++) {
        if (PT_UINT8 == iter.value()) {
            ui->combo_box_param_type->setCurrentText(iter.key());
        }
    }

    //算法类型与算法名映射
    m_mapAlgorithmName[QStringLiteral("随机选择算法")] =EA_Random;
    m_mapAlgorithmName[QStringLiteral("随机移位亦或算法")] =EA_ShitfOxr;
    m_mapAlgorithmName[QStringLiteral("分解成商与余数算法")] =EA_Mod;
    QStringList stringListAlgorithm;
    for (auto iter = m_mapAlgorithmName.begin(); iter != m_mapAlgorithmName.end(); iter++) {
        stringListAlgorithm << iter.key();
    }
    ui->combo_box_encrypt_algorithm->addItems(stringListAlgorithm);
    for (auto iter = m_mapAlgorithmName.begin(); iter != m_mapAlgorithmName.end(); iter++) {
        if (EA_Random == iter.value()) {
            ui->combo_box_encrypt_algorithm->setCurrentText(iter.key());
        }
    }



    //每一秒触发一次
    //connect(&m_timerChange2Array, &QTimer::timeout, this, &string_random_dec::OnChange2Arrary);
    //m_timerChange2Array.start(TIMER_INTERVAL_CHANGE); // 开启
}

string_random_dec::~string_random_dec()
{
    delete ui;
}

//转换成数组
void string_random_dec::OnStartEncrypt() {
    //获取
    QString qstrSrc = ui->text_edit_src->toPlainText();
    QString qstrParam = ui->line_edit_param_name->text();
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
    //处理编码
    QByteArray byteArraySrcTmp =  qstrSrc.toUtf8();
    if(PT_UINT16 == m_mapParamType[ui->combo_box_param_type->currentText()]) {
       byteArraySrcTmp = utf8BaToUtf16Ba(byteArraySrcTmp);
    }
    //根据算法选择不同
    EncryptAlgorithm  nEncryptAlgo = m_mapAlgorithmName[ui->combo_box_encrypt_algorithm->currentText()];
    if (EA_Random == nEncryptAlgo) {//随机算法的话，就做一次随机
        quint32 u32Rand = QRandomGenerator::global()->bounded(quint32(EA_Count - EA_Random - 1));
        nEncryptAlgo = static_cast<EncryptAlgorithm>(u32Rand + EA_ShitfOxr);
    }

    //加密
    QString qstrEncData;
    switch (nEncryptAlgo) {
    case EA_ShitfOxr: {
         qstrEncData = self_define_enc_utils::StringRandomShiftOxr(byteArraySrcTmp, qstrParam);
    }
        break;
    case EA_Mod: {
        qstrEncData = self_define_enc_utils::StringRandomMod(byteArraySrcTmp, qstrParam);
    }
        break;
    default: {
        sched_mngt_message_box::question(QStringLiteral("错误加密算法"), sched_mngt_message_box::MBT_INFO);
    }
        break;
    }



    QString qstrRst = qstrComment + qstrEncData;
    ui->text_edit_dst->setText(qstrRst);
    //
    m_qstrPriv = qstrRst;
}


//修改类型
void string_random_dec::ParamTypeChanged(const QString &) {
    OnStartEncrypt();
}

//修改算法
void string_random_dec::AlgorithmChanged(const QString &) {
    OnStartEncrypt();
}
