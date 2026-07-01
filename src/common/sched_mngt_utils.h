#ifndef SCHED_MNGT_UTILS_H
#define SCHED_MNGT_UTILS_H

#include <QString>
#include <iostream>
#include <QMouseEvent>
#include <QCursor>
#include <utility>
#include <QFile>
#include <QTextStream>

//将QString转换为C++的std::string.编码是UTF8
static std::string GetStdUtf8String(const QString &qstr) {
    QByteArray utf8Array = qstr.toUtf8();
    std::string strTmp(utf8Array.constData(), utf8Array.size());
    strTmp.append(1, '\0');
    int nSize = strTmp.size();
    strTmp.resize(nSize - 1);
    return strTmp;
}


/**
 * @brief UTF-8 QByteArray 转 UTF-16 QByteArray
 * @param utf8Ba  输入：UTF-8编码的QByteArray
 * @param isLittleEndian  是否小端（true=UTF-16LE，false=UTF-16BE）
 * @return  UTF-16编码的QByteArray
 */
static QByteArray utf8BaToUtf16Ba(const QByteArray& utf8Ba, bool isLittleEndian = true)
{
    // 步骤1：UTF-8 → QString（原生UTF-16）
    QString utf16Str = QString::fromUtf8(utf8Ba);
    if (utf16Str.isEmpty()) {
        return QByteArray();
    }

    // 步骤2：提取UTF-16字节流（const ushort* 转 const char*）
    const ushort* utf16Data = utf16Str.utf16(); // 返回UTF-16编码的ushort指针
    int utf16ByteLen = utf16Str.length() * 2;   // 每个UTF-16字符占2字节
    QByteArray utf16Ba(reinterpret_cast<const char*>(utf16Data), utf16ByteLen);

    // 步骤3：处理字节序（若需大端，反转每个字符的字节）
    if (!isLittleEndian) {
        for (int i = 0; i < utf16Ba.size(); i += 2) {
            //std::swap(utf16Ba[i], utf16Ba[i+1]);
        }
    }
    return utf16Ba;
}


// 兼容Qt 5/Qt 6的全局鼠标位置获取函数
static QPoint getGlobalMousePos(QMouseEvent *event)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    return event->globalPosition().toPoint();
#else
    return event->globalPos();
#endif
}

// 兼容Qt5/Qt6获取全局鼠标位置
static QPoint getGlobalMousePos()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    // Qt6：QApplication::mousePosition()返回QPointF，需转QPoint
    return QApplication::mousePosition().toPoint();
#else
    // Qt5：直接使用QCursor::pos()
    return QCursor::pos();
#endif
}

//将一个QString按照utf8编码写入文件
static bool WriteData2File(const QString& filePath, const QByteArray &qstr) {
    // 1. 创建文件对象并以"只写+覆盖"模式打开
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    // 3. 写入内容
     qint64 writtenBytes = file.write(qstr);

    // 4. 刷新并关闭文件（QFile析构时会自动关闭，但显式关闭更规范）
    file.flush();
    file.close();
    if (writtenBytes != qstr.size()) {
        return false;
    }
    // 检查是否有写入错误
    if (file.error() != QFile::NoError) {
        return false;
    }
    return true;
}

#endif // SCHED_MNGT_UTILS_H
