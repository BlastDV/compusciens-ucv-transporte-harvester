#include "driversmanager.h"
#include "ui_driversmanager.h"

DriversManager::DriversManager(QWidget *parent) : QWidget(parent), ui(new Ui::DriversManager)
{
    ui->setupUi(this);

    // Inicializamos el indice de registros
    RegIndex= -1;

    // Iniciamos el conector de la BD
    ConnectionName= "DriversManager";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;

    // Llenamos la interfaz de informacion
    LoadInitialData();
}

DriversManager::~DriversManager()
{
    delete ui;
}

// Esta funcion llenara los inputs con los datos contenidos en "input"
void DriversManager::FillInputs(QSqlQuery Input)
{
    ui->Nombre1Input->setText(Input.value(0).toString());
    ui->Nombre2Input->setText(Input.value(1).toString());
    ui->Apellido1Input->setText(Input.value(2).toString());
    ui->Apellido2Input->setText(Input.value(3).toString());
    ui->CedulaInput->setText(Input.value(5).toString());

}

// Llena la interfaz con toda la informacion respectiva de la BD
void DriversManager::LoadInitialData()
{
    // Vamos a crear una conexion para poder traer ciertos datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
                              "No se ha podido recuperar la lista de transportistas. "
                              "Revise el estado de la Base de Datos.\n\nMensaje: Error DBA1\n"+
                              Connector->getLastError().text());
    }
    else
    {
        // Connector->Connector le dice a Infoquery con cual BD y conexion funcionar
        QSqlQuery* DataQuery= new QSqlQuery (Connector->Connector);

        if(!DataQuery->exec(QString("SELECT primer_apellido, cedula FROM transportista")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
                                  "No se ha podido recuperar la lista de transportistas. "
                                  "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ1\n");
        }
        else
        {

            QStringList AuxLastNameList;
            DataQuery->first();

            // Vamos a llenar la lista de apellidos auxiliar
            do
            {
                AuxLastNameList<<DataQuery->value(0).toString();
                qDebug("Adding: %s", AuxLastNameList.last().toStdString().c_str());
            }
            while (DataQuery->next());

            // Ahora la ordenamos
            AuxLastNameList.sort();

            // Y ahora vamos a proceder a llenar el mapa de cedulas y la lista de apellidos
            // para la navegacion
            DataQuery->first();
            for (int i=0; i<AuxLastNameList.count(); i++)
            {


                qDebug(">%s", AuxLastNameList.at(i).toStdString().c_str());
            }



            // Llenemos los campos
            //FillInputs(*DataQuery);
        }
    }
}
