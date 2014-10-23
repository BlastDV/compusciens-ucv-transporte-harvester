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

    // Llenamos la interfaz
    LoadData();
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

// Este metodo recupera la informacion de la BD y los guarda en estructuras
// locales para poder mostrarlas.
void RoutesManager::LoadData()
{
    // Vamos a crear una conexion para poder traer ciertos datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
                              "No se ha podido recuperar la lista de rutas. "
                              "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBA1<br>"+
                              Connector->getLastError().text());
    }
    else
    {
        // Connector->Connector le dice a Infoquery con cual BD y conexion funcionar
        QSqlQuery* DataQuery= new QSqlQuery (Connector->Connector);

        if(!DataQuery->exec(QString("SELECT origen, destino, id FROM ruta")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
                                  "No se ha podido recuperar la lista de rutas. "
                                  "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBQ1<br>");
        }
        else
        {
            //EraseData();

            DataQuery->first();

            if (DataQuery->isValid())
            {
                // Primero vamos a llenar los inputs con el primer registro
                FillInputs(*DataQuery);
/*
                // Para el QMap de usuarios
                int k=0;

                // Ahora llenemos la lista de usuarios
                do
                {
                    ui->UsersList->insertRow(ui->UsersList->rowCount());
                    ui->UsersList->setRowHeight(ui->UsersList->rowCount()-1, 20);

                    QTableWidgetItem *Usuario;
                    Usuario= new QTableWidgetItem (DataQuery->value(0).toString()); // El id
                    Usuario->setFlags(Usuario->flags() ^ Qt::ItemIsEditable); // No editable

                    // Insertamos en la lista
                    ui->UsersList->setItem(ui->UsersList->rowCount()-1, 0, Usuario);
                    // Y agregamos al QMap
                    UsersLocalList.insert(k, DataQuery->value(0).toString());

                    // Siguiente indice de usuario
                    k++;
                }
                while(DataQuery->next());

                // Arreglemos un aspecto visual de la lista
                ui->UsersList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

                // Y ponemos RegIndex a apuntar al primer registro
                RegIndex= 0;*/
            }
        }

        Connector->EndConnection();
    }
}

// Este metodo llenara los distintos inputs con la informacion contenida en input
void RoutesManager::FillInputs(QSqlQuery Input)
{
    ui->RouteStartInput->setText(Input.value(0).toString());
    ui->RouteEndInput->setText(Input.value(1).toString());
    ui->RouteIDInput->setText(Input.value(2).toString());
}
