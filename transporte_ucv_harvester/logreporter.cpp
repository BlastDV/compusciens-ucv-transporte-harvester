#include "logreporter.h"
#include "ui_logreporter.h"

LogReporter::LogReporter(QWidget *parent) : QWidget(parent), ui(new Ui::LogReporter)
{
    ui->setupUi(this);
    UserID= "default";

    ConnectionName= "LogReporter";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;

    PermissionRep= new PermissionReporter ();
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
        "No se ha podido podido acceder a la base de datos, revise el estado"
        "de la misma.\n\nMensaje: Error DBA1\n"+ Connector->getLastError().text());

        return false;
    }
    else
    {
        // No importa el nombre de usuario sino sus permisos
        QSqlQuery* Rightsquery= new QSqlQuery (Connector->Connector);
        if (!Rightsquery->exec(QString("SELECT permisos FROM usuarios WHERE id=")+QString("'")+UserID+QString("'")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
            "No se han podido determinar sus permisos. Revise el estado "
            "de la Base de Datos\n\nMensaje: Error DBQ1\n"+ Connector->getLastError().text());

            return false;
        }
        else
        {
            Rightsquery->first();

            // Obtenidos los permisos, vamos a mostrar los eventos registrados

            QSqlQuery* Eventsquery= new QSqlQuery (Connector->Connector);
            if (!Eventsquery->exec(QString("SELECT * FROM actividades WHERE usuario=")+QString("'")+UserID+QString("'")))
            {
                QMessageBox::critical(0, QObject::tr("Error"),
                "No se han podido leer los eventos. Revise el estado "
                "de la Base de Datos\n\nMensaje: Error DBQ2\n"+ Connector->getLastError().text());

                return false;
            }
            else
            {
                Eventsquery->first();

                do
                {
                    ui->EventsList->insertRow(ui->EventsList->rowCount());
                    ui->EventsList->setRowHeight(ui->EventsList->rowCount()-1, 20);

                    QTableWidgetItem *Usuario, *Evento, *Tiempo;

                    Usuario= new QTableWidgetItem (Eventsquery->value(0).toString()); // Texto
                    Usuario->setFlags(Usuario->flags() ^ Qt::ItemIsEditable); // No editable

                    Tiempo= new QTableWidgetItem (Eventsquery->value(1).toString()); // Texto
                    Tiempo->setFlags(Tiempo->flags() ^ Qt::ItemIsEditable); // No editable

                    Evento= new QTableWidgetItem (Eventsquery->value(2).toString()); // Texto
                    Evento->setFlags(Evento->flags() ^ Qt::ItemIsEditable); // No editable

                    ui->EventsList->setItem(ui->EventsList->rowCount()-1, 0, Usuario);
                    ui->EventsList->setItem(ui->EventsList->rowCount()-1, 1, Tiempo);
                    ui->EventsList->setItem(ui->EventsList->rowCount()-1, 2, Evento);

                    /*QTableWidgetItem test= *ui->EventsList->itemAt(0,1);
                    test.setText("Ola ke ase");*/
                    //QMessageBox::critical(0, QObject::tr("=)"),
                    //"Ola ke ase");
                    //ui->EventsList->rowAt(1)


                }
                while (Eventsquery->next());

                //ui->EventsList->resizeColumnsToContents();
                ui->EventsList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
            }


        }
    }
    return true;
}
