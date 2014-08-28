#include "usersmanager.h"
#include "ui_usersmanager.h"

UsersManager::UsersManager(QWidget *parent) : QMainWindow(parent), ui(new Ui::UsersManager)
{
    ui->setupUi(this);

    // Ningun usuario nos ve por ahora
    UserID= "default";

    // Este sera nuestro informante de permisos
    PermissionRep= new PermissionReporter(this);

    // No estamos editando
    EDITING= false;

    // Iniciamos el conector de la BD
    ConnectionName= "UsersManager";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;

    // Unimos las conexiones entre widgets
    connect(ui->UsersList, SIGNAL(cellClicked(int,int)), this, SLOT(UpdateView(int,int)));

    // Llenamos la interfaz
    LoadData();

    // Y la mostramos de forma preliminar, esto es: vacia. Por razones de seguridad
    // en posibles usos incorrectos de esta clase. Debe llamarse UpdateUser() antes de mostrarla!!!
    //SetView("default");

    //bypass temporal
    SetView("Inicio");
}

UsersManager::~UsersManager()
{
    delete ui;
}

// Este metodo recupera la informacion de la BD y los guarda en estructuras
// locales para poder mostrarlas.
void UsersManager::LoadData()
{
    // Vamos a crear una conexion para poder traer ciertos datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
                              "No se ha podido recuperar la lista de usuarios. "
                              "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBA1<br>"+
                              Connector->getLastError().text());
    }
    else
    {
        // Connector->Connector le dice a Infoquery con cual BD y conexion funcionar
        QSqlQuery* DataQuery= new QSqlQuery (Connector->Connector);

        if(!DataQuery->exec(QString("SELECT id, permisos FROM usuario")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
                                  "No se ha podido recuperar la lista de usuarios. "
                                  "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBQ1<br>");
        }
        else
        {
            EraseData();

            DataQuery->first();

            if (DataQuery->isValid())
            {
                // Primero vamos a llenar los inputs con el primer registro
                FillInputs(*DataQuery);

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

            }
        }

        Connector->EndConnection();
    }
}

// Este metodo limpia las estructuras locales
void UsersManager::EraseData()
{

}

// Este metodo llenara los distintos inputs con la informacion contenida en input
void UsersManager::FillInputs(QSqlQuery Input)
{
    // Creamos un informante de permisos auxiliar (NO TIENE QUE VER CON EL
    // INFORMANTE DE LA CLASE)
    PermissionReporter PReporter;

    ui->IDInput->setText(Input.value(0).toString());
    PReporter.CalculatePermissions(Input.value(1).toString());

    //Obtenidos los permisos, pasemos a llenar los checkbox
    ui->CanReadUsersInput->setChecked(PReporter.CanReadUsersManager());
    ui->CanCreateUsersInput->setChecked(PReporter.CanCreateUsers());
    ui->CanEditUsersInput->setChecked(PReporter.CanEditUsers());
    ui->CanDeleteUsersInput->setChecked(PReporter.CanDeleteUsers());
    ui->CanReadAllLogInput->setChecked(PReporter.CanReadAllLog());
    ui->CanDeleteLogInput->setChecked(PReporter.CanWriteLog());
    ui->CanReadDriversInput->setChecked(PReporter.CanReadDriversManager());
    ui->CanCreateDriversInput->setChecked(PReporter.CanCreateDrivers());
    ui->CanEditDriversInput->setChecked(PReporter.CanEditDrivers());
    ui->CanSuspendDriversInput->setChecked(PReporter.CanSuspendDrivers());
    ui->CanDeleteDriversInput->setChecked(PReporter.CanDeleteDrivers());

}

// Este metodo borrara todos los inputs de la interfaz
void UsersManager::EraseInputs()
{
    ui->IDInput->setText("");
    ui->CanReadUsersInput->setChecked(false);
    ui->CanCreateUsersInput->setChecked(false);
    ui->CanEditUsersInput->setChecked(false);
    ui->CanDeleteUsersInput->setChecked(false);
    ui->CanReadAllLogInput->setChecked(false);
    ui->CanDeleteLogInput->setChecked(false);
    ui->CanReadDriversInput->setChecked(false);
    ui->CanCreateDriversInput->setChecked(false);
    ui->CanEditDriversInput->setChecked(false);
    ui->CanSuspendDriversInput->setChecked(false);
    ui->CanDeleteDriversInput->setChecked(false);
}

// Este metodo manipulara la interfaz de acuerdo a ciertos criterios como
// el usuario actual y sus respectivos permisos
void UsersManager::SetView(QString Modalidad)
{
    if (Modalidad=="default")
    {
        ui->TopFrame->hide();
        ui->ActionsFrame->hide();
    }
    else
    if (Modalidad=="Inicio")
    {
        // Se muestran los bloques de la interfaz ahora que la clase ha
        // sido usada correctamente.
        ui->TopFrame->show();
        ui->ActionsFrame->show();

        // Bloqueo de Inputs
        ui->CanReadUsersInput->setEnabled(false);
        ui->CanCreateUsersInput->setEnabled(false);
        ui->CanEditUsersInput->setEnabled(false);
        ui->CanDeleteUsersInput->setEnabled(false);
        ui->CanReadAllLogInput->setEnabled(false);
        ui->CanDeleteLogInput->setEnabled(false);
        ui->CanReadDriversInput->setEnabled(false);
        ui->CanCreateDriversInput->setEnabled(false);
        ui->CanEditDriversInput->setEnabled(false);
        ui->CanSuspendDriversInput->setEnabled(false);
        ui->CanDeleteDriversInput->setEnabled(false);

        // Bloqueo de Acciones
        ui->SaveRegButton->hide();
        ui->CancelButton->hide();
    }
}

// Esta funcion permitira que al hacer click en un usuario de la lista, se muestre
// su informacion en la interfaz
void UsersManager::UpdateView(int UserIndex, int Trash)
{
    // Ignoramos "Trash", es solo algo necesario por la arquitectura de Qt

    // En UserIndex tenemos el indice del usuario en el QMap que el <usuario> quiere consultar

    // Vamos a crear una conexion para poder traer ciertos datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
                              "No se ha podido recuperar la lista de usuarios. "
                              "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBA1<br>"+
                              Connector->getLastError().text());
    }
    else
    {
        // Connector->Connector le dice a Infoquery con cual BD y conexion funcionar
        QSqlQuery* DataQuery= new QSqlQuery (Connector->Connector);

        if(!DataQuery->exec(QString("SELECT id, permisos FROM usuario WHERE id='")+UsersLocalList.value(UserIndex)+QString("'")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
                                  "No se ha podido recuperar la lista de usuarios. "
                                  "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBQ1<br>");
        }
        else
        {
            // Llegados a este punto,actualicemos la interfaz
            DataQuery->first();
            FillInputs(*DataQuery);
        }

        Connector->EndConnection();
    }
}
