#ifndef FILE_TO_ARRAY_H
#define FILE_TO_ARRAY_H

#include <QWidget>
#include <QMap>

namespace Ui {
class file_to_array;
}

enum FileEncryptAlgorithm {
    FEA_NO_ENCRYPT //不加密
    , FEA_Random //在几个算法中随机
    , FEA_ShitfOxr //移位亦或加密算法
    , FEA_Mod //分解成整除与余数两个字节


    , FEA_Count
};


class file_to_array : public QWidget
{
    Q_OBJECT

public:
    explicit file_to_array(QWidget *parent = nullptr);
    ~file_to_array();

private:
    //开始转
    void Start();
private:
    Ui::file_to_array *ui;

    //映射
    QMap<QString, FileEncryptAlgorithm> m_mapAlgorithmName;
};

#endif // FILE_TO_ARRAY_H
