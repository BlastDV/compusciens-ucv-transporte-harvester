#include "logmaster.h"
#include <QMessageBox>

LogMaster::LogMaster(QObject *parent) : QObject(parent)
{
    ConnectionName= "LogMaster";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;
}

// Este procedimiento permitira registrar los eventos en la tabla correspondiente
// de la BD. Sus pasos son sencillos y se remiten a:
// -Abrir una conexion a la BD
// -Insertar el Evento
// -Cerrar la conexion
bool LogMaster::RegistrarEvento(QString evento)
{
    bool REGISTROOK= false;

    if (!Connector->RequestConnection())
        REGISTROOK= false;
    else
    {
        // Si la conexion pudo abrirse, se procede a insertar el evento
        // Este se compone de un usuario, un tiempo y una actividad
        QSqlQuery InsertQuery;
        if (!InsertQuery.exec(QString("INSERT INTO actividades (usuario, tiempo, actividad) VALUES (")+
                              QString("'")+UserID+QString("', '")+QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")+QString("', ")+
                              QString("'")+evento+QString("')")))
            REGISTROOK= false;
        else
            REGISTROOK= true;

        Connector->EndConnection();
    }

    return REGISTROOK;
}

bool LogMaster::UpdateUser(QString user)
{
    UserID= user;

    return true;
}
