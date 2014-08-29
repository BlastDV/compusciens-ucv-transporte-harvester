#include "logmaster.h"
#include <QMessageBox>

LogMaster::LogMaster(QObject *parent) : QObject(parent)
{
    ConnectionName= "LogMaster";
    Connector= new DBConnector(this);

    // No reportamos de nadie!
    UserID= "default-user";

    // Este valor por default amerita que el objeto padre llame al metodo SetConnectionName de esta
    // clase antes de empezar a reportar sus eventos
    Connector->ConnectionName= "default-logger";
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
        QSqlQuery* InsertQuery= new QSqlQuery (Connector->Connector);
        if (!InsertQuery->exec(QString("INSERT INTO actividad (usuario, tiempo, actividad) VALUES (")+
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

// Esta funcion se encargara de actualizar el nombre de conexion de
// la clase
void LogMaster::SetConnectionName(QString CName)
{
    ConnectionName= CName;
    Connector->ConnectionName= ConnectionName;
}

QString LogMaster::getUserID()
{
    return UserID;
}
