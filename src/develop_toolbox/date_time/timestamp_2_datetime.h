#ifndef TIMESTAMP_2_DATETIME_H
#define TIMESTAMP_2_DATETIME_H

#include <QWidget>
#include <QMap>

namespace Ui {
class timestamp_2_datetime;
}

class timestamp_2_datetime : public QWidget
{
    Q_OBJECT

public:
    explicit timestamp_2_datetime(QWidget *parent = nullptr);
    ~timestamp_2_datetime();

private:
    //时间戳转日期
    void OnTimestamp2Date();
    //日期转时间戳
    void OnDate2Timestamp();
    //初始化时区
    void OnInitTimeZone();
    //时区的时间间隔，UTC为准。输出方便理解的字符串 (UTC+XX:XX)
    QString TimeZoneOffset(int nUtcOffset);
private:
    //
    Ui::timestamp_2_datetime *ui;
    //显示名和时区名的对应关系
    QMap<QString, QString> m_mapShow2TimeZoneName;
};

#endif // TIMESTAMP_2_DATETIME_H
