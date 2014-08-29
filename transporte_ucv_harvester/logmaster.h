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
    void SetConnectionName(QString CName);
    
public slots:
    bool RegistrarEvento (QString evento);
    bool UpdateUser (QString user);
    QString getUserID();

private:
    DBConnector* Connector;
    QString UserID;

signals:
};

#endif // LOGMASTER_H
