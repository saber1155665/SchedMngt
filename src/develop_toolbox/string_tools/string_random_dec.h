#ifndef STRING_RANDOM_DEC_H
#define STRING_RANDOM_DEC_H

#include <QWidget>
#include <QTimer>
#include <QByteArray>
#include <QMap>

namespace Ui {
class string_random_dec;
}

class string_random_dec : public QWidget
{
    Q_OBJECT

    enum ParamType{
        PT_UINT8,
        PT_UINT16,

        PT_COUNT
    };


    enum EncryptAlgorithm {
        EA_Random //在几个算法中随机
        , EA_ShitfOxr //移位亦或加密算法
        , EA_Mod //分解成整除与余数两个字节


        , EA_Count
    };

public:
    explicit string_random_dec(QWidget *parent = nullptr);
    ~string_random_dec();

private:
    //转换成数组
    void OnStartEncrypt();


    //修改类型
    void ParamTypeChanged(const QString &);
    //修改算法
    void AlgorithmChanged(const QString &);
private:
    //
    Ui::string_random_dec *ui;
    //定时检测，自动更新数组
    QTimer m_timerChange2Array;
    //记录上一次内容
    QString m_qstrPriv;
    //映射
    QMap<QString, ParamType> m_mapParamType;
    //映射
    QMap<QString, EncryptAlgorithm> m_mapAlgorithmName;
};

#endif // STRING_RANDOM_DEC_H
