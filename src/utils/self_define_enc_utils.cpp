#include "self_define_enc_utils.h"


#include <QRandomGenerator>
#include <QVector>
#include <QDebug>

self_define_enc_utils::self_define_enc_utils()
{

}


//随机移位然后异或算法
QString self_define_enc_utils::StringRandomShiftOxr(const QByteArray& byteArraySrc, const QString &qstrParamsName) {
    QVector<quint8> vectDst;
    quint8 randomShift = static_cast<quint8>(QRandomGenerator::global()->bounded(quint32(6)));
    randomShift += 1; //移位1到7位之间随机

    // 生成一个在0到99之间的随机整数KEY
    quint32 randomInt1 = QRandomGenerator::global()->bounded(quint32(255));
    quint32 randomInt2 = QRandomGenerator::global()->bounded(quint32(255));
    quint32 randomInt3 = QRandomGenerator::global()->bounded(quint32(255));
    quint32 randomInt4 = QRandomGenerator::global()->bounded(quint32(255));
    QVector<quint8> vectRandom;
    vectRandom.push_back(static_cast<quint8>(randomInt1));
    vectRandom.push_back(static_cast<quint8>(randomInt2));
    vectRandom.push_back(static_cast<quint8>(randomInt3));
    vectRandom.push_back(static_cast<quint8>(randomInt4));

    //亦或移位加密
    for (int i = 0; i < byteArraySrc.length(); i++) {
        quint8 u8Tmp = byteArraySrc[i] ^ vectRandom[i % vectRandom.size()];
        u8Tmp = (u8Tmp >> randomShift) | (u8Tmp << (8 - randomShift));
        vectDst.push_back(u8Tmp);
    }
    //转字符串
    QString qstrHexArray = Bin2Hex(vectDst, 4);
    QString qstrRandomKey = Bin2Hex(vectRandom, 0);

    //输出日志
    qDebug() << QStringLiteral("加密结果:") << qstrHexArray;
    qDebug() << QStringLiteral("移位次数:") << randomShift;
    qDebug() << QStringLiteral("KEY:") << qstrRandomKey;

    //测试解密代码
    //uint8_t xxx[]= {0xCB, 0x5F, 0xB5, 0x77, 0x0A, 0xB6, 0x2D, 0xE5, 0xCA, 0x5E, 0xB5, 0x77, 0x03, 0xEE, 0xD5, 0x0};
    //uint8_t u8ArryKey[] = {0x28, 0xB8, 0xC2, 0x9C, 0x0};
    //uint8_t u8Shift = 5 ;
    //for (int i = 0; i < sizeof(xxx) - 1; i++) {
    //    uint8_t u8Tmp = (xxx[i] << u8Shift) | (xxx[i] >> (8 - u8Shift));
    //    u8Tmp = u8Tmp ^ u8ArryKey[i % (sizeof(u8ArryKey)  - 1)];
    //    xxx[i] = u8Tmp ;
    //}
    //生成代码模板
    QString qstrCodeFormat = QStringLiteral("uint8_t u8Arry$PARAM_NAME$Data[]=$HEX_ARRAY$;\r\n"
                                            "uint8_t u8Arry$PARAM_NAME$Key[] = $HEX_KEY$;\r\n"
                                            "uint8_t u8$PARAM_NAME$Shift = $HEX_SHIFT$; \r\n"
                                            "for (int i = 0; i < sizeof(u8Arry$PARAM_NAME$Data) - 4; i++) { \r\n"
                                            "   uint8_t u8Tmp = (u8Arry$PARAM_NAME$Data[i] << u8$PARAM_NAME$Shift) | (u8Arry$PARAM_NAME$Data[i] >> (8 - u8$PARAM_NAME$Shift)); \r\n"
                                            "   u8Tmp = u8Tmp ^  u8Arry$PARAM_NAME$Key[i % (sizeof( u8Arry$PARAM_NAME$Key))]; \r\n"
                                            "   u8Arry$PARAM_NAME$Data[i] = u8Tmp ; \r\n"
                                            "} \r\n");
    qstrCodeFormat.replace("$HEX_ARRAY$", qstrHexArray);
    qstrCodeFormat.replace("$HEX_KEY$", qstrRandomKey);
    qstrCodeFormat.replace("$HEX_SHIFT$", QString("%1").arg(randomShift));
    qstrCodeFormat.replace("$PARAM_NAME$", qstrParamsName);
    return qstrCodeFormat;
}


//分解成整除与余数两个字节
QString self_define_enc_utils::StringRandomMod(const QByteArray& byteArraySrc, const QString &qstrParamsName) {
    QVector<quint8> vectDst;
    quint8 randomShift = static_cast<quint8>(QRandomGenerator::global()->bounded(quint32(6)));
    randomShift += 1; //移位1到7位之间随机

    // 生成一个在1到126之间的随机整数KEY
    quint32 randomInt1 = 1 + QRandomGenerator::global()->bounded(quint32(126));
    quint32 randomInt2 = 1 + QRandomGenerator::global()->bounded(quint32(126));
    quint32 randomInt3 = 1 + QRandomGenerator::global()->bounded(quint32(126));
    quint32 randomInt4 = 1 + QRandomGenerator::global()->bounded(quint32(126));
    QVector<quint8> vectRandom;
    vectRandom.push_back(static_cast<quint8>(randomInt1));
    vectRandom.push_back(static_cast<quint8>(randomInt2));
    vectRandom.push_back(static_cast<quint8>(randomInt3));
    vectRandom.push_back(static_cast<quint8>(randomInt4));

    //分解成模与余数两个值
    for (int i = 0; i < byteArraySrc.length(); i++) {
        quint8 u8Mod = byteArraySrc[i] % vectRandom[i % 4];
        quint8 u8Quot = byteArraySrc[i] / vectRandom[i % 4];
        vectDst.push_back(u8Quot);
        vectDst.push_back(u8Mod);
    }
    //转字符串
    QString qstrHexArray = Bin2Hex(vectDst, 4);
    QString qstrRandomKey = Bin2Hex(vectRandom, 0);

    //测试解密代码
   //int nIndex = 0;
   //uint8_t u8ArryData[]={ 0x08, 0x08, 0x02, 0x01, 0x01, 0x14, 0x01, 0x1D, 0x02, 0x08, 0x01, 0x06, 0x00, 0x20, 0x00, 0x27
   //                , 0x03, 0x01, 0x02, 0x05, 0x00, 0x27, 0x00, 0x20, 0x09, 0x04, 0x02, 0x01, 0x01, 0x13, 0x01, 0x1D
   //                , 0x02, 0x08, 0x01, 0x06, 0x00, 0x20, 0x00, 0x27, 0x03, 0x01, 0x01, 0x2D, 0x0, 0x0, 0x0, 0x0};;
   //uint8_t u8ArryKey[] = { 0x0C, 0x37, 0x5F, 0x57};
   //for (nIndex = 0; nIndex < sizeof(u8ArryData) / 2; nIndex++) {
   //    quint8 u8 = u8ArryData[nIndex * 2] * u8ArryKey[nIndex % 4] + u8ArryData[nIndex * 2 + 1];
   //    u8ArryData[nIndex] = u8;
   //}
   //u8ArryData[nIndex] = '\0';

    QString qstrCodeFormat = QStringLiteral("int i32$PARAM_NAME$Index = 0;\r\n"
                                            "uint8_t u8Arry$PARAM_NAME$Data[]=$HEX_ARRAY$;\r\n"
                                            "uint8_t u8Arry$PARAM_NAME$Key[] = $HEX_KEY$;\r\n"
                                            "for (i32$PARAM_NAME$Index = 0; i32$PARAM_NAME$Index < sizeof(u8Arry$PARAM_NAME$Data) / 2; i32$PARAM_NAME$Index++) {\r\n"
                                            "   uint8_t u8 = u8Arry$PARAM_NAME$Data[i32$PARAM_NAME$Index * 2] * u8Arry$PARAM_NAME$Key[i32$PARAM_NAME$Index % 4] + u8Arry$PARAM_NAME$Data[i32$PARAM_NAME$Index * 2 + 1];\r\n"
                                            "   u8Arry$PARAM_NAME$Data[i32$PARAM_NAME$Index] = u8;\r\n"
                                            "}\r\n"
                                            "u8Arry$PARAM_NAME$Data[i32$PARAM_NAME$Index] = \'\\0\';\r\n"
                                            );
    qstrCodeFormat.replace("$HEX_ARRAY$", qstrHexArray);
    qstrCodeFormat.replace("$HEX_KEY$", qstrRandomKey);
     qstrCodeFormat.replace("$PARAM_NAME$", qstrParamsName);
    return qstrCodeFormat;
}

//向量转C语言16禁止字符串，且每16个字符换一行
QString self_define_enc_utils::Bin2Hex(const QVector<quint8> &vect, const int &nAppendZero) {
    QString qstrRst = "{";
    QString qstrFormat = QStringLiteral(", 0x%02X");
    QByteArray byteArrayFormat = qstrFormat.toUtf8();
    if (1 == vect.size()) {
        return QString();
    }
    //第一行特别处理，方便后面使用逗号
    qstrRst  += QString::asprintf(QStringLiteral(" 0x%02X").toUtf8(),  vect[0]);

    for (int i = 1; i < vect.size(); i++) {
        if ((0 != i) && (0 == (i % 16))) {
            qstrRst += "\r\n                ";
        }
       // 格式化为两位16进制字符串（不足补零）
       QString qstrHex = QString::asprintf(byteArrayFormat.data(), vect[i]);
       qstrRst += qstrHex;
    }
    for (int i = 0; i < nAppendZero; i++) {
         qstrRst += ", 0x0";
    }
    //末尾的\0
    qstrRst += "};";
    return qstrRst;
}

//向量转C语言16禁止字符串，且每16个字符换一行
QString self_define_enc_utils::Bin2Hex(const QByteArray& byteArraySrc, const int &nAppendZero) {
    QString qstrRst = "{";
    QString qstrFormat = QStringLiteral(", 0x%02X");
    QByteArray byteArrayFormat = qstrFormat.toUtf8();
    if (1 == byteArraySrc.size()) {
        return QString();
    }

    const QByteArray byteHex = byteArraySrc.toHex().toUpper();
    //第一行特别处理，方便后面使用逗号
    qstrRst  += "0x" + byteHex.mid(0, 2);

    for (int i = 1; i < byteArraySrc.size(); i++) {
        if ((0 != i) && (0 == (i % 16))) {
            qstrRst += "\r\n                ";
        }
       // 格式化为两位16进制字符串（不足补零）
       QString qstrHex =", 0x" + byteHex.mid(i * 2, 2);  QString::asprintf(byteArrayFormat.data(), byteArraySrc[i]);
       qstrRst += qstrHex;
    }
    for (int i = 0; i < nAppendZero; i++) {
         qstrRst += ", 0x0";
    }
    //末尾的\0
    qstrRst += "};";
    return qstrRst;
}


void GetEncrpytData(unsigned char **pszBuf) {
    *pszBuf = (unsigned char *)malloc(sizeof(char));
}

//随机移位然后异或算法
QString self_define_enc_utils::BinaryRandomShiftOxr(const QByteArray& byteArraySrc, const QString &qstrParamsName) {
    QVector<quint8> vectDst;
    quint8 randomShift = static_cast<quint8>(QRandomGenerator::global()->bounded(quint32(6)));
    randomShift += 1; //移位1到7位之间随机

    // 生成一个在0到99之间的随机整数KEY
    quint32 randomInt1 = QRandomGenerator::global()->bounded(quint32(255));
    quint32 randomInt2 = QRandomGenerator::global()->bounded(quint32(255));
    quint32 randomInt3 = QRandomGenerator::global()->bounded(quint32(255));
    quint32 randomInt4 = QRandomGenerator::global()->bounded(quint32(255));
    QVector<quint8> vectRandom;
    vectRandom.push_back(static_cast<quint8>(randomInt1));
    vectRandom.push_back(static_cast<quint8>(randomInt2));
    vectRandom.push_back(static_cast<quint8>(randomInt3));
    vectRandom.push_back(static_cast<quint8>(randomInt4));

    uint32_t nLength = static_cast<uint32_t>(byteArraySrc.length());
    QByteArray byteArrayPrefix;
    byteArrayPrefix.append(char(nLength & 0xFF));
    byteArrayPrefix.append(char((nLength >> 8) & 0xFF));
    byteArrayPrefix.append(char((nLength >> 16) & 0xFF));
    byteArrayPrefix.append(char((nLength >> 24) & 0xFF));
    QByteArray byteArrayData = byteArrayPrefix + byteArraySrc;

    //亦或移位加密
    for (int i = 0; i < byteArrayData.length(); i++) {
        quint8 u8Tmp = byteArrayData[i] ^ vectRandom[i % vectRandom.size()];
        u8Tmp = (u8Tmp >> randomShift) | (u8Tmp << (8 - randomShift));
        vectDst.push_back(u8Tmp);
    }
    //转字符串
    QString qstrHexArray = Bin2Hex(vectDst, 4);
    QString qstrRandomKey = Bin2Hex(vectRandom, 0);

    //输出日志
    qDebug() << QStringLiteral("加密结果:") << qstrHexArray;
    qDebug() << QStringLiteral("移位次数:") << randomShift;
    qDebug() << QStringLiteral("KEY:") << qstrRandomKey;

    //测试解密代码
    //uint8_t xxx[]= {0xCB, 0x5F, 0xB5, 0x77, 0x0A, 0xB6, 0x2D, 0xE5, 0xCA, 0x5E, 0xB5, 0x77, 0x03, 0xEE, 0xD5, 0x0};
    //uint8_t u8ArryKey[] = {0x28, 0xB8, 0xC2, 0x9C, 0x0};
    //uint8_t u8Shift = 5 ;
    //for (int i = 0; i < sizeof(xxx) - 1; i++) {
    //    uint8_t u8Tmp = (xxx[i] << u8Shift) | (xxx[i] >> (8 - u8Shift));
    //    u8Tmp = u8Tmp ^ u8ArryKey[i % (sizeof(u8ArryKey)  - 1)];
    //    xxx[i] = u8Tmp ;
    //}
    //生成代码模板
    QString qstrCodeFormat = QStringLiteral(
                "#pragma once\r\n\r\n"
                "#include <stdint.h>\r\n"
                "#include <stdlib.h>\r\n"
                "static uint8_t g_u8Arry$PARAM_NAME$Data[]=$HEX_ARRAY$;\r\n"
                "\r\n\r\n"
                "static void Get$PARAM_NAME$EncrpytData(uint8_t **psz$PARAM_NAME$Buf) {\r\n"
                "    *psz$PARAM_NAME$Buf = (uint8_t *)malloc(sizeof(g_u8Arry$PARAM_NAME$Data));\r\n"
                "    if (NULL == *psz$PARAM_NAME$Buf) return;\r\n"
                "    uint8_t *u8Arry$PARAM_NAME$Data = g_u8Arry$PARAM_NAME$Data;\r\n"
                "    uint8_t u8Arry$PARAM_NAME$Key[] = $HEX_KEY$;\r\n"
                "    uint8_t u8$PARAM_NAME$Shift = $HEX_SHIFT$; \r\n"
                "    for (int i = 0; i < sizeof(g_u8Arry$PARAM_NAME$Data) - 4; i++) { \r\n"
                "       uint8_t u8Tmp = (u8Arry$PARAM_NAME$Data[i] << u8$PARAM_NAME$Shift) | (u8Arry$PARAM_NAME$Data[i] >> (8 - u8$PARAM_NAME$Shift)); \r\n"
                "       u8Tmp = u8Tmp ^  u8Arry$PARAM_NAME$Key[i % (sizeof( u8Arry$PARAM_NAME$Key))]; \r\n"
                "       (*psz$PARAM_NAME$Buf)[i] = u8Tmp ; \r\n"
                "    } \r\n"
                "}"
                );
    qstrCodeFormat.replace("$HEX_ARRAY$", qstrHexArray);
    qstrCodeFormat.replace("$HEX_KEY$", qstrRandomKey);
    qstrCodeFormat.replace("$HEX_SHIFT$", QString("%1").arg(randomShift));
    qstrCodeFormat.replace("$PARAM_NAME$", qstrParamsName);
    return qstrCodeFormat;
}


//分解成整除与余数两个字节
QString self_define_enc_utils::BinaryRandomMod(const QByteArray& byteArraySrc, const QString &qstrParamsName) {
    QVector<quint8> vectDst;
    quint8 randomShift = static_cast<quint8>(QRandomGenerator::global()->bounded(quint32(6)));
    randomShift += 1; //移位1到7位之间随机

    // 生成一个在1到126之间的随机整数KEY
    quint32 randomInt1 = 1 + QRandomGenerator::global()->bounded(quint32(126));
    quint32 randomInt2 = 1 + QRandomGenerator::global()->bounded(quint32(126));
    quint32 randomInt3 = 1 + QRandomGenerator::global()->bounded(quint32(126));
    quint32 randomInt4 = 1 + QRandomGenerator::global()->bounded(quint32(126));
    QVector<quint8> vectRandom;
    vectRandom.push_back(static_cast<quint8>(randomInt1));
    vectRandom.push_back(static_cast<quint8>(randomInt2));
    vectRandom.push_back(static_cast<quint8>(randomInt3));
    vectRandom.push_back(static_cast<quint8>(randomInt4));

    uint32_t nLength = static_cast<uint32_t>(byteArraySrc.length());
    QByteArray byteArrayPrefix;
    byteArrayPrefix.append(char(nLength & 0xFF));
    byteArrayPrefix.append(char((nLength >> 8) & 0xFF));
    byteArrayPrefix.append(char((nLength >> 16) & 0xFF));
    byteArrayPrefix.append(char((nLength >> 24) & 0xFF));
    QByteArray byteArrayData = byteArrayPrefix + byteArraySrc;

    //分解成模与余数两个值
    for (int i = 0; i < byteArrayData.length(); i++) {
        quint8 u8Mod = byteArrayData[i] % vectRandom[i % 4];
        quint8 u8Quot = byteArrayData[i] / vectRandom[i % 4];
        vectDst.push_back(u8Quot);
        vectDst.push_back(u8Mod);
    }
    //转字符串
    QString qstrHexArray = Bin2Hex(vectDst, 4);
    QString qstrRandomKey = Bin2Hex(vectRandom, 0);

    //测试解密代码
   //int nIndex = 0;
   //uint8_t u8ArryData[]={ 0x08, 0x08, 0x02, 0x01, 0x01, 0x14, 0x01, 0x1D, 0x02, 0x08, 0x01, 0x06, 0x00, 0x20, 0x00, 0x27
   //                , 0x03, 0x01, 0x02, 0x05, 0x00, 0x27, 0x00, 0x20, 0x09, 0x04, 0x02, 0x01, 0x01, 0x13, 0x01, 0x1D
   //                , 0x02, 0x08, 0x01, 0x06, 0x00, 0x20, 0x00, 0x27, 0x03, 0x01, 0x01, 0x2D, 0x0, 0x0, 0x0, 0x0};;
   //uint8_t u8ArryKey[] = { 0x0C, 0x37, 0x5F, 0x57};
   //for (nIndex = 0; nIndex < sizeof(u8ArryData) / 2; nIndex++) {
   //    quint8 u8 = u8ArryData[nIndex * 2] * u8ArryKey[nIndex % 4] + u8ArryData[nIndex * 2 + 1];
   //    u8ArryData[nIndex] = u8;
   //}
   //u8ArryData[nIndex] = '\0';

    QString qstrCodeFormat = QStringLiteral(
                "#pragma once\r\n\r\n"
                "#include <stdint.h>\r\n"
                "#include <stdlib.h>\r\n"
                "static uint8_t g_u8Arry$PARAM_NAME$Data[]=$HEX_ARRAY$;\r\n"
                "\r\n\r\n"
                "static void Get$PARAM_NAME$EncrpytData(uint8_t **psz$PARAM_NAME$Buf) {\r\n"
                "    *psz$PARAM_NAME$Buf = (uint8_t *)malloc(sizeof(g_u8Arry$PARAM_NAME$Data));\r\n"
                "   if (NULL == *psz$PARAM_NAME$Buf) return;\r\n"
                "   int i32$PARAM_NAME$Index = 0;\r\n"
                "   uint8_t *u8Arry$PARAM_NAME$Data=g_u8Arry$PARAM_NAME$Data;\r\n"
                "   uint8_t u8Arry$PARAM_NAME$Key[] = $HEX_KEY$;\r\n"
                "   for (i32$PARAM_NAME$Index = 0; i32$PARAM_NAME$Index < sizeof(g_u8Arry$PARAM_NAME$Data) / 2; i32$PARAM_NAME$Index++) {\r\n"
                "      uint8_t u8 = u8Arry$PARAM_NAME$Data[i32$PARAM_NAME$Index * 2] * u8Arry$PARAM_NAME$Key[i32$PARAM_NAME$Index % 4] + u8Arry$PARAM_NAME$Data[i32$PARAM_NAME$Index * 2 + 1];\r\n"
                "      (*psz$PARAM_NAME$Buf)[i32$PARAM_NAME$Index] = u8;\r\n"
                "   }\r\n"
                "   (*psz$PARAM_NAME$Buf)[i32$PARAM_NAME$Index] = \'\\0\';\r\n"
                "}\r\n"
                );
    qstrCodeFormat.replace("$HEX_ARRAY$", qstrHexArray);
    qstrCodeFormat.replace("$HEX_KEY$", qstrRandomKey);
     qstrCodeFormat.replace("$PARAM_NAME$", qstrParamsName);
    return qstrCodeFormat;
}

