#include "logreporter.h"
#include "ui_logreporter.h"

LogReporter::LogReporter(QWidget *parent) : QWidget(parent), ui(new Ui::LogReporter)
{
    ui->setupUi(this);
    UserID= "default";

    ConnectionName= "LogReporter";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;
}

LogReporter::~LogReporter()
{
    delete ui;
}


// Esta funcion actualiza el usuario de la clase
bool LogReporter::UpdateUser(QString user)
{
    UserID= user;
    return true;
}

/* Esta funcion sera la encargada de determinar los permisos del usuario
 * actual y en base a estos determinar cuales eventos y opciones seran
 * mostrados */
bool LogReporter::LoadEvents()
{
    // Ya hay un usuario especificado?
    if (UserID=="default")
        return false;

    // Abramos una conexion a la BD
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, QObject::tr("Error"),
        "No se han podido leer los eventos, revise el estado"
        "de la Base de Datos\n\nMensaje: Error DBA1\n"+ Connector->getLastError().text());

        return false;
    }
    else
    {
        // No importa el nombre de usuario sino sus permisos
        QSqlQuery* Rightsquery= new QSqlQuery (Connector->Connector);
        if (!Rightsquery->exec(QString("SELECT permisos FROM usuarios WHERE id=")+QString("'")+UserID+QString("'")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
            "No se han podido determinar sus permisos. Revise el estado"
            "de la Base de Datos\n\nMensaje: Error DBQ1\n"+ Connector->getLastError().text());

            return false;
        }
        else
        {
            Rightsquery->first();

            ui->EventsList->insertRow(0);
            ui->EventsList->setRowHeight(0, 30);
            QMessageBox::critical(0, QObject::tr("=)"),
            "Ola ke ase");
            //ui->EventsList->rowAt(1)
        }
    }

}
