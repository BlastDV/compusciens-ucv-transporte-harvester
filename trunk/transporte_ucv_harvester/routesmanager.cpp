#include "routesmanager.h"
#include "ui_routesmanager.h"

RoutesManager::RoutesManager(QWidget *parent) : QWidget(parent), ui(new Ui::RoutesManager)
{
    ui->setupUi(this);

    // Ningun usuario nos ve por ahora
    UserID= "default";

    // Este sera nuestro informante de permisos
    PermissionRep= new PermissionReporter(this);

    // Iniciamos el conector de la BD
    ConnectionName= "RoutesManager";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;

    // Este sera nuestro link con el log
    Logger= new LogMaster(this);
    Logger->SetConnectionName(ConnectionName + "-Logger");


}

RoutesManager::~RoutesManager()
{
    delete ui;
}

// Esta funcion se encargara de leer los permisos del usuario actual y tomar decisiones en base a eso
bool RoutesManager::UpdateUser (QString user)
{
    UserID= user;

    // Abramos una conexion a la BD
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, QObject::tr("Error"),
                              "No se ha podido podido acceder a la base de datos, revise el estado "
                              "de la misma.<br><br>Mensaje: Error DBA1<br>"+ Connector->getLastError().text());

        return false;
    }
    else
    {
        // No importa el nombre de usuario sino sus permisos
        QSqlQuery* Rightsquery= new QSqlQuery (Connector->Connector);
        if (!Rightsquery->exec(QString("SELECT permisos FROM usuario WHERE id=")+QString("'")+UserID+QString("'")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
                                  "No se han podido determinar sus permisos. Revise el estado "
                                  "de la Base de Datos<br><br>Mensaje: Error DBQ1<br>"+ Connector->getLastError().text());

            return false;
        }
        else
        {
            Rightsquery->first();

            // Obtenidos los permisos, vamos a decodificarlos
            PermissionRep->CalculatePermissions(Rightsquery->value(0).toString());
            //SetView("Inicio");

            // Y cambiamos el UserID del logger
            Logger->UpdateUser(UserID);

            // Listos para usar!
        }

        Connector->EndConnection();
        return true;
    }
}
