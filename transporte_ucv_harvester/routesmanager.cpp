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

    // Unimos las conexiones entre widgets
    connect(ui->RoutesList, SIGNAL(cellClicked(int,int)), this, SLOT(UpdateView(int, int)));

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

// Este metodo limpiara las estructuras para solo almacenar lo necesario
void RoutesManager::EraseData()
{
    RoutesLocalList.clear();
    StopsLocalList.clear();
}

// Esto solo borrara las paradas de la ruta actual para que no se muestren en la proxima ruta
void RoutesManager::EraseStops()
{
    StopsLocalList.clear();
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
            EraseData();

            DataQuery->first();
            int FirstRouteID= 0; // Esto para actualizar las paradas mas adelante

            if (DataQuery->isValid())
            {
                // Primero vamos a llenar los inputs con el primer registro
                FillRouteInputs(*DataQuery);
                FirstRouteID= DataQuery->value(2).toInt();

                // Para el QMap de rutas
                int k=0;

                // Ahora llenemos la lista de rutas
                do
                {
                    ui->RoutesList->insertRow(ui->RoutesList->rowCount());
                    ui->RoutesList->setRowHeight(ui->RoutesList->rowCount()-1, 20);

                    QTableWidgetItem *Ruta;
                    QString RouteName= DataQuery->value(0).toString() + "-" + DataQuery->value(1).toString();
                    Ruta= new QTableWidgetItem (RouteName); // El id
                    Ruta->setFlags(Ruta->flags() ^ Qt::ItemIsEditable); // No editable

                    // Insertamos en la lista
                    ui->RoutesList->setItem(ui->RoutesList->rowCount()-1, 0, Ruta);
                    // Y agregamos al QMap
                    RoutesLocalList.insert(k, DataQuery->value(2).toString());

                    // Siguiente indice de Ruta
                    k++;
                }
                while(DataQuery->next());

                // Arreglemos un aspecto visual de la lista
                ui->RoutesList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

                // Y ponemos RegIndex a apuntar al primer registro
                //RegIndex= 0;

                // Y llenamos la interfaz con la informacion de la primera parada de la primera ruta
                LoadStops(FirstRouteID);
            }
        }

        Connector->EndConnection();
    }
}

// Esta funcion permitira que al hacer click en una ruta de la lista, se muestre
// su informacion en la interfaz
void RoutesManager::UpdateView(int RouteIndex, int Trash)
{
    // Ignoramos "Trash", es solo algo necesario por la arquitectura de Qt

    // En UserIndex tenemos el indice de la ruta en el QMap que el usuario quiere consultar
    // Por eso actualizamos RegIndex
    //RegIndex= UserIndex;

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

        if(!DataQuery->exec(QString("SELECT origen, destino, id FROM ruta WHERE id='")+RoutesLocalList.value(RouteIndex)+QString("'")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
                                  "No se ha podido recuperar la lista de rutas. "
                                  "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBQ1<br>");
        }
        else
        {
            // Llegados a este punto,actualicemos la interfaz
            DataQuery->first();
            FillRouteInputs(*DataQuery);
            LoadStops(DataQuery->value(2).toInt());
        }

        Connector->EndConnection();
    }
}

// Este metodo llenara los distintos inputs con la informacion contenida en input
void RoutesManager::FillRouteInputs(QSqlQuery Input)
{
    ui->RouteStartInput->setText(Input.value(0).toString());
    ui->RouteEndInput->setText(Input.value(1).toString());
    ui->RouteIDInput->setText(Input.value(2).toString());
}

void RoutesManager::LoadStops(int routeindex)
{
    // Vamos a crear una conexion para poder traer ciertos datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
                              "No se ha podido recuperar la lista de paradas. "
                              "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBA1<br>"+
                              Connector->getLastError().text());
    }
    else
    {
        // Connector->Connector le dice a Infoquery con cual BD y conexion funcionar
        QSqlQuery* DataQuery= new QSqlQuery (Connector->Connector);

        if(!DataQuery->exec(QString("SELECT id, nombre, descripcion FROM parada WHERE ruta_id=%1").arg(routeindex)))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
                                  "No se ha podido recuperar la lista de paradas. "
                                  "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBQ1<br>");
        }
        else
        {
            EraseStops();

            // Llegados a este punto, llenemos el QMap de paradas para la rutas respectiva
            DataQuery->first();

            if (DataQuery->isValid())
            {
                // Primero vamos a llenar los inputs con el primer registro
                FillStopInputs(*DataQuery);

                // Para el QMap de paradas
                int k=0;

                // Ahora llenemos la lista de paradas
                do
                {
                    // Insertamos en la lista
                    ui->StopsList->addItem(DataQuery->value(1).toString());

                    // Y agregamos al QMap
                    StopsLocalList.insert(k, DataQuery->value(0).toString());

                    // Siguiente indice de Parada
                    k++;
                }
                while(DataQuery->next());

                // Y ponemos RegIndex a apuntar al primer registro
                //RegIndex= 0;
            }
        }

        Connector->EndConnection();
    }
}

void RoutesManager::FillStopInputs(QSqlQuery Input)
{
    ui->StopIDInput->setText(Input.value(0).toString());
    ui->StopNameInput->setText(Input.value(1).toString());
    ui->StopDescriptionInput->setText(Input.value(2).toString());
}
