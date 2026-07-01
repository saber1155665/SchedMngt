#ifndef ADD_LOG_INFO_H
#define ADD_LOG_INFO_H

#include <QObject>

class add_log_info : public QObject
{
    Q_OBJECT
public:
    explicit add_log_info(QObject *parent = nullptr);

signals:

};

#endif // ADD_LOG_INFO_H
