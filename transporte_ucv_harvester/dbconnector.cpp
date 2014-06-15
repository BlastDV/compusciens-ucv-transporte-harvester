#include "dbconnector.h"

DBConnector::DBConnector(QObject *parent) : QObject(parent)
{

}

bool DBConnector::RequestConnection()
{
    bool APERTURA;

    Connector= QSqlDatabase::addDatabase("QODBC");
    Connector.setHostName("localhost");
    Connector.setDatabaseName("ptransporteucv");
    Connector.setUserName("compusciens");
    Connector.setPassword("kakolukiya");
    Connector.setPort(3306);

    APERTURA= Connector.open();

    return APERTURA;
}

void DBConnector::EndConnection()
{
    Connector.close();
    Connector.removeDatabase("ptransporteucv");
}

QSqlError DBConnector::getLastError()
{
    return Connector.lastError();
}
