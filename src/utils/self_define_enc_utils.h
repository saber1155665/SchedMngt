#ifndef SELF_DEFINE_ENC_UTILS_H
#define SELF_DEFINE_ENC_UTILS_H

#include <qstring.h>
#include <qvector.h>

class self_define_enc_utils
{
public:
    self_define_enc_utils();

    //向量转C语言16禁止字符串，且每16个字符换一行
    static QString Bin2Hex(const QVector<quint8> &vect, const int &nAppendZero);
    static QString Bin2Hex(const QByteArray& byteArraySrc, const int &nAppendZero);

    //随机移位然后异或算法
    static QString StringRandomShiftOxr(const QByteArray& byteArraySrc, const QString &qstrParamsName);
    //分解成整除与余数两个字节
    static QString StringRandomMod(const QByteArray& byteArraySrc, const QString &qstrParamsName);


    //随机移位然后异或算法
    static QString BinaryRandomShiftOxr(const QByteArray& byteArraySrc, const QString &qstrParamsName);
    //分解成整除与余数两个字节
    static QString BinaryRandomMod(const QByteArray& byteArraySrc, const QString &qstrParamsName);
};

#endif // SELF_DEFINE_ENC_UTILS_H
