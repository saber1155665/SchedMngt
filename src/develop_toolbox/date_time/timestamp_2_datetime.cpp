#include "timestamp_2_datetime.h"
#include "ui_timestamp_2_datetime.h"

#include <QDateTime>
#include <QTimeZone>
#include <QCompleter>

timestamp_2_datetime::timestamp_2_datetime(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::timestamp_2_datetime)
{
    ui->setupUi(this);
    //绑定
    connect(ui->btn_change_2_date, &QPushButton::clicked, this, &timestamp_2_datetime::OnTimestamp2Date);
    connect(ui->btn_change_2_timestamp, &QPushButton::clicked, this, &timestamp_2_datetime::OnDate2Timestamp);
    //设置时区
    OnInitTimeZone();
    //设置单位
    QStringList stringListUnit;
    stringListUnit << QStringLiteral("秒")
                   << QStringLiteral("毫秒");
    ui->combo_box_unit->addItems(stringListUnit);
    ui->combo_box_unit_2->addItems(stringListUnit);
}

timestamp_2_datetime::~timestamp_2_datetime()
{
    delete ui;
}

//时间戳转日期
void timestamp_2_datetime::OnTimestamp2Date() {
    //得到时间戳
    QString qstrTimestamp = ui->line_edit_timestamp->text();
    //得到单位
    QString qstrUnit = ui->combo_box_unit->currentText();
    //得到时区
    QString qstrTimezone = ui->combo_box_time_zone->currentText();
    //开始转换
    qint64 i64Timestamp = qstrTimestamp.toLongLong();
    if (QStringLiteral("秒") ==  qstrUnit) {
        i64Timestamp *= 1000;
    }
    //时区
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(i64Timestamp);
    QDateTime dstTimeZoneDateTime = dateTime.toTimeZone(QTimeZone(m_mapShow2TimeZoneName[qstrTimezone].toUtf8()));
    QString qstrDateTime = dstTimeZoneDateTime.toString("yyyy-MM-dd HH:mm:ss");
    //设置结果
    ui->line_edit_result_date->setText(qstrDateTime);
}

//日期转时间戳
void timestamp_2_datetime::OnDate2Timestamp() {
    QString qstDateTime = ui->line_edit_date_time->text();
    //得到单位
    QString qstrUnit = ui->combo_box_unit->currentText();
    //得到时区
    QString qstrTimezone = ui->combo_box_time_zone_2->currentText();
    //转换
    QDateTime dateTime = QDateTime::fromString(qstDateTime, "yyyy-MM-dd HH:mm:ss");
    dateTime.setTimeZone(QTimeZone(m_mapShow2TimeZoneName[qstrTimezone].toUtf8()));
    qint64 i64Timestamp = 0;
    if (QStringLiteral("秒") ==  qstrUnit) {
        i64Timestamp = dateTime.toSecsSinceEpoch();
    } else {
        i64Timestamp = dateTime.toMSecsSinceEpoch();
    }
    //设置结果
    ui->line_edit_result_timestamp->setText(QString("%1").arg(i64Timestamp));
}


//初始化时区
void timestamp_2_datetime::OnInitTimeZone() {
    //初始化时区
    QDateTime referenceTime = QDateTime::currentDateTime(); // 可以是任何时间点
    QList<QByteArray> listByteTimezones = QTimeZone::availableTimeZoneIds(); 
    QStringList listStrEastTimeZone;//东部时区+
    QStringList listStrWsetTimeZone;//西部时区-
    for (auto iter = listByteTimezones.begin(); iter != listByteTimezones.end(); iter++) {
        //得到相对UTC的时区差。由于时区差值，不一定都是整数。比如印度.因此需要分钟
        QString qstrTmpTimeZone = QString(*iter);
        QTimeZone tmpTz(qstrTmpTimeZone.toUtf8());
        int nUtcOffset = tmpTz.offsetFromUtc(referenceTime);
        //连接市区差和时区名
        QString qstrText = TimeZoneOffset(nUtcOffset) + QString(*iter);
        m_mapShow2TimeZoneName[qstrText] = qstrTmpTimeZone;
        if (nUtcOffset >= 0) {
            listStrEastTimeZone.push_back(qstrText);
        } else {
            listStrWsetTimeZone.push_back(qstrText);
        }
    }
    //排序.东部时区从小到大
    std::sort(listStrEastTimeZone.begin(), listStrEastTimeZone.end(), [](const QString &a, const QString &b) {
            return a < b; // 对于QString的比较，可以直接使用 > 运算符
        });
    //排序，西部时区从大到小
    std::sort(listStrWsetTimeZone.begin(), listStrWsetTimeZone.end(), [](const QString &a, const QString &b) {
            return a > b; // 对于QString的比较，可以直接使用 > 运算符
        });
    //连接结果
    QStringList listStrTimeZone;
    listStrTimeZone.append(listStrWsetTimeZone);
    listStrTimeZone.append(listStrEastTimeZone);
    //添加
    ui->combo_box_time_zone->addItems(listStrTimeZone);
    ui->combo_box_time_zone_2->addItems(listStrTimeZone);
    //选中当前时区
    QTimeZone currentTimeZone = QTimeZone::systemTimeZone();
    QString qstrCurrentTimeZone =  QString(currentTimeZone.id());
    QString qstrShowCurrentTimeZone;
    for (auto iter = m_mapShow2TimeZoneName.begin(); iter != m_mapShow2TimeZoneName.end(); iter++) {
        if (iter.value() == qstrCurrentTimeZone) {
            qstrShowCurrentTimeZone = iter.key();
            break;
        }
    }
    ui->combo_box_time_zone->setCurrentText(qstrShowCurrentTimeZone);
    ui->combo_box_time_zone_2->setCurrentText(qstrShowCurrentTimeZone);
    //设置模糊搜索
    QCompleter *pCompleter = new QCompleter(listStrTimeZone, this);
    pCompleter->setFilterMode(Qt::MatchContains); // 模糊匹配
    pCompleter->setCaseSensitivity(Qt::CaseInsensitive);//忽略大小写
    ui->combo_box_time_zone->lineEdit()->setCompleter(pCompleter);

    QCompleter *pCompleter_2 = new QCompleter(listStrTimeZone, this);
    pCompleter_2->setFilterMode(Qt::MatchContains); // 模糊匹配
    pCompleter_2->setCaseSensitivity(Qt::CaseInsensitive);//忽略大小写
    ui->combo_box_time_zone_2->lineEdit()->setCompleter(pCompleter_2);

}

//时区的时间间隔，UTC为准。输出方便理解的字符串 (UTC+XX:XX)
QString timestamp_2_datetime::TimeZoneOffset(int nUtcOffset) {
    //转换成为分钟数
    int nMinuteTotal = nUtcOffset / 60;
    //得到时和分
    int nMinute = nMinuteTotal % 60;
    int nHour = nMinuteTotal / 60;

    QString qstrRst = QString("(UTC%1%2:%3)").arg(nHour >= 0 ? "+" : "-").arg(abs(nHour), 2, 10, QLatin1Char('0'))
            .arg(nMinute, 2, 10, QLatin1Char('0'));
    return qstrRst;
}
