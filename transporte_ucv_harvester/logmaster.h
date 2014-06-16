#ifndef LOGMASTER_H
#define LOGMASTER_H

#include <QObject>
#include <QtSql>
#include "dbconnector.h"

class LogMaster : public QObject
{
    Q_OBJECT
public:
    explicit LogMaster(QObject *parent = 0);

    QString ConnectionName;
    
public slots:
    bool RegistrarEvento (QString evento);
    bool UpdateUser (QString user);

private:
    DBConnector* Connector;
    QString UserID;

signals:
};

#endif // LOGMASTER_H
