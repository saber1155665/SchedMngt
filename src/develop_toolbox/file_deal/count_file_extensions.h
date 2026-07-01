#ifndef COUNT_FILE_EXTENSIONS_H
#define COUNT_FILE_EXTENSIONS_H

#include <QWidget>

namespace Ui {
class count_file_extensions;
}

class count_file_extensions : public QWidget
{
    Q_OBJECT

public:
    explicit count_file_extensions(QWidget *parent = nullptr);
    ~count_file_extensions();

private:
    //开始统计
    void StateCount();
    // 统计文件后缀数量
    QMap<QString, int> countFileExtensions(const QString& dirPath, bool caseSensitive);

private:
    Ui::count_file_extensions *ui;
};

#endif // COUNT_FILE_EXTENSIONS_H
