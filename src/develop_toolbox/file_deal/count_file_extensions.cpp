#include "count_file_extensions.h"
#include "ui_count_file_extensions.h"
#include "sched_mngt_message_box.h"

#include <QDir>
#include <QFileInfoList>
#include <QMap>
#include <QDebug>
#include <QString>


#define TABLE_COL_SIZE 2

count_file_extensions::count_file_extensions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::count_file_extensions)
{
    ui->setupUi(this);


    connect(ui->push_button_start, &QPushButton::clicked, this, &count_file_extensions::StateCount);

    QStringList strListHeader;
    strListHeader << QStringLiteral("文件类型")
                  << QStringLiteral("数量");
    ui->table_count_extension->setColumnCount(TABLE_COL_SIZE); //列数为2
    ui->table_count_extension->setHorizontalHeaderLabels(strListHeader);
    ui->table_count_extension->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->table_count_extension->setAutoFillBackground(false);
    //设置高宽自适应
    for (int i = 0; i < strListHeader.size(); i++) {
         ui->table_count_extension->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
}

count_file_extensions::~count_file_extensions()
{
    delete ui;
}


void count_file_extensions::StateCount() {
    QString qstrDir = ui->line_edit_search_path->text();
    if (qstrDir.isEmpty()) {
        sched_mngt_message_box::question(QStringLiteral("directory path is empty"), sched_mngt_message_box::MBT_INFO);
        return;
    }
    bool bCaseSensitive = ui->radio_button_case_sensitive->isChecked();

    //先清除数据
    ui->table_count_extension->setRowCount(0);
    //统计数据
    auto mapCountExtension = countFileExtensions(qstrDir, bCaseSensitive);
    //显示
    for (auto iter = mapCountExtension.begin(); iter != mapCountExtension.end(); iter++) {
        // 末尾插入一行
        int row = ui->table_count_extension->rowCount();
        ui->table_count_extension->insertRow(row);
        ui->table_count_extension->setItem(row, 0, new QTableWidgetItem(iter.key()));
        ui->table_count_extension->setItem(row, 1, new QTableWidgetItem(QString::number(iter.value())));
    }
}


// 统计文件后缀数量
QMap<QString, int> count_file_extensions::countFileExtensions(const QString& dirPath, bool caseSensitive)
{
    QMap<QString, int> extCount;
    QDir dir(dirPath);

    if (!dir.exists()) {
        return extCount;
    }

    // 只获取 当前目录 的文件和目录
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& info : list) {

        // ==============================
        // 如果是目录，并且允许递归
        // 手动调用自己！！！
        // ==============================
        if (info.isDir()) {
            // 递归调用，把子目录结果合并进来
            QMap<QString, int> subMap = countFileExtensions(info.absoluteFilePath(), caseSensitive);
            for (auto it = subMap.begin(); it != subMap.end(); ++it) {
                extCount[it.key()] += it.value();
            }
            continue; // 目录不统计后缀
        }

        // ==============================
        // 处理文件
        // ==============================
        QString ext = info.suffix();
        if (!caseSensitive) {
            ext = ext.toLower();
        }
        if (ext.isEmpty()) {
            ext = QStringLiteral("无后缀");
        }
        extCount[ext]++;
    }

    return extCount;
}
