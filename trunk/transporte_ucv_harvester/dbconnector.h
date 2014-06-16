#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <QObject>
#include <QtSql>

class DBConnector : public QObject
{
    Q_OBJECT
public:
    explicit DBConnector(QObject *parent = 0);

    bool RequestConnection ();
    void EndConnection ();
    QSqlError getLastError ();
    QString ConnectionName;
    QSqlDatabase Connector;
    
public slots:

private:


signals:
    
};

#endif // DBCONNECTOR_H
