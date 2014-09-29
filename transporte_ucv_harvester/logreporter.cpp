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

    connect(ui->UserFilter, SIGNAL(currentIndexChanged(QString)), this, SLOT(FilterUsers(QString)));
    connect(ui->EventsList, SIGNAL(cellClicked(int,int)), this, SLOT(MarkRow(int,int)));

}

LogReporter::~LogReporter()
{
    delete ui;
}

// Esta funcion actualiza el usuario de la clase
bool LogReporter::UpdateUser(QString user)
{
    UserID= user;

    // Una vez que tenemos usuario, cargamos los eventos
    LoadEvents();

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
        if (!Rightsquery->exec(QString("SELECT permisos FROM usuario WHERE id=")+QString("'")+UserID+QString("'")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
            "No se han podido determinar sus permisos. Revise el estado "
            "de la Base de Datos\n\nMensaje: Error DBQ1\n"+ Connector->getLastError().text());

            return false;
        }
        else
        {
            Rightsquery->first();

            // Obtenidos los permisos, vamos a decodificarlos
            PermissionRep->CalculatePermissions(Rightsquery->value(0).toString());

            // Ahora, de acuerdo a lo obtenido, determinemos que vamos a mostrar

            // UserID puede borrar entradas?
            if (!PermissionRep->CanWriteLog())
                ui->DeleteFrame->hide();
            else
                ui->DeleteFrame->show();

            // Esta declaracion sera comun para cualquier combinacion de permisos
            QSqlQuery* Eventsquery= new QSqlQuery (Connector->Connector);
            QString Query;

            // Determinemos lo que vamos a traer de la base de datos

            // UserID puede leer todo el log?
            if (PermissionRep->CanReadAllLog())
            {
                Query= QString("SELECT * from actividad");

                ui->UsersFrame->show(); // Le mostramos la opcion Filtrar
            }
            else
            {
                Query= QString("SELECT * FROM actividad WHERE usuario=")+QString("'")+UserID+QString("'");

                ui->UsersFrame->hide(); // No lo dejamos ver la opcion Filtrar
            }

            if (!Eventsquery->exec(Query))
            {
                QMessageBox::critical(0, QObject::tr("Error"),
                "No se han podido leer los eventos. Revise el estado "
                "de la Base de Datos\n\nMensaje: Error DBQ2\n"+ Connector->getLastError().text());

                return false;
            }
            else
            {
                Eventsquery->first();

                if (Eventsquery->isValid())
                {
                    // Esta sera la lista de usuarios distintos en los eventos
                    QStringList Userlist;

                    do
                    {
                        ui->EventsList->insertRow(ui->EventsList->rowCount());
                        ui->EventsList->setRowHeight(ui->EventsList->rowCount()-1, 20);

                        QTableWidgetItem *Usuario, *Evento, *Tiempo;

                        Usuario= new QTableWidgetItem (Eventsquery->value(1).toString()); // Texto
                        Usuario->setFlags(Usuario->flags() ^ Qt::ItemIsEditable); // No editable

                        QStringList TimeOutput= Eventsquery->value(2).toString().split("T");
                        Tiempo= new QTableWidgetItem (TimeOutput.at(0)+"\t"+TimeOutput.at(1)); // Texto
                        Tiempo->setFlags(Tiempo->flags() ^ Qt::ItemIsEditable); // No editable

                        Evento= new QTableWidgetItem (Eventsquery->value(3).toString()); // Texto
                        Evento->setFlags(Evento->flags() ^ Qt::ItemIsEditable); // No editable

                        // Insertamos el evento en la tabla
                        ui->EventsList->setItem(ui->EventsList->rowCount()-1, 0, Usuario);
                        ui->EventsList->setItem(ui->EventsList->rowCount()-1, 1, Tiempo);
                        ui->EventsList->setItem(ui->EventsList->rowCount()-1, 2, Evento);

                        // Y en la lista de eventos local
                        EventsLocalList.append(Eventsquery->value(0).toInt());

                        // El usuario puede ver todo el log?
                        if (PermissionRep->CanReadAllLog())
                        {
                            // Hemos agregado el usuario a la lista de filtros?
                            if (!Userlist.contains(Usuario->text()))
                                Userlist.append(Usuario->text());
                        }

                    }
                    while (Eventsquery->next());

                    // Llenemos la lista de filtros si esta disponible
                    if (PermissionRep->CanReadAllLog())
                    {
                        for (int i=0; i< Userlist.count(); i++)
                        {
                            ui->UserFilter->addItem(Userlist.at(i));
                        }
                    }
                }

                // Ahora actualizamos aspectos visuales y de informacion
                if (ui->EventsList->rowCount()==1)
                    ui->CounterLabel->setText("1 evento");
                else
                    ui->CounterLabel->setText(QString("%1 eventos").arg(ui->EventsList->rowCount()));

                //ui->EventsList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
                ui->EventsList->horizontalHeader()->setStretchLastSection(true); //e(->setSectionResizeMode(1, QHeaderView::Stretch);

            }
        }
        Connector->EndConnection();
    }
    return true;
}

/* Esta funcion permitira filtrar los resultados de acuerdo a lo deseado por el usuario,
 * siempre en caso de que pueda ver todos los eventos */
void LogReporter::FilterUsers(QString userid)
{
    if (userid!="Todos")
    {
        QList <QTableWidgetItem*> Users;

        Users= ui->EventsList->findItems(userid, Qt::MatchExactly);

        // Primero escodemos todo
        for (int i=0; i<ui->EventsList->rowCount(); i++)
            ui->EventsList->hideRow(i);

        // Y luego mostramos aquellos que nos interesa filtrar
        for (int i=0; i<Users.count(); i++)
            ui->EventsList->showRow(Users.at(i)->row());

        // Finalmente, actualizemos la informacion visual
        if (Users.count()==1)
            ui->CounterLabel->setText("1 evento");
        else
            ui->CounterLabel->setText(QString("%1 eventos").arg(Users.count()));
    }
    else
    {
        // Simplemente mostremos todos los eventos
        for (int i=0; i<ui->EventsList->rowCount(); i++)
            ui->EventsList->showRow(i);

        // Finalmente, actualizemos la informacion visual
        if (ui->EventsList->rowCount()==1)
            ui->CounterLabel->setText("1 evento");
        else
            ui->CounterLabel->setText(QString("%1 eventos").arg(ui->EventsList->rowCount()));
    }

    /* Nota, esta no es la mejor forma de realizar esto, puesto que la clase QTableView
     * provee mejores metodos para tal fin. */
}

/* Esto se activara cuando se decida eliminar un registro del log */
void LogReporter::on_DeleteButton_clicked()
{
    // Primero vamos a determinar cuantos eventos hay seleccionados
    QList <QTableWidgetItem*> Seleccionados;
    Seleccionados= ui->EventsList->selectedItems();

    if (Seleccionados.count()<1)
    {
        QMessageBox::warning(0, QObject::tr("Error"),
        "No ha seleccionado ningun evento");
    }
    else
    {
        QMessageBox Question;

        /* ADVERTENCIA: ESTOS MENSAJES SON UNA FUENTE DE AMBIGUEDAD EN POTENCIA */
        if (Seleccionados.count()/ui->EventsList->columnCount()==1)
        {
            Question.setText("1 evento seleccionado");
            Question.setInformativeText("¿Desea eliminarlo?");
        }
        else
        {
            Question.setText(QString("%1 eventos seleccionados").arg(Seleccionados.count()/ui->EventsList->columnCount()));
            Question.setInformativeText("¿Desea eliminarlos?");
        }

        Question.addButton("Cancelar", QMessageBox::RejectRole);
        Question.addButton("Si", QMessageBox::YesRole);

        // Si el usuario confirma la accion, seguimos
        if (Question.exec()==1)
        {
            // Abramos una conexion a la BD
            if (!Connector->RequestConnection())
            {
                QMessageBox::critical(0, QObject::tr("Error"),
                "No se ha podido podido acceder a la base de datos, revise el estado"
                "de la misma.\n\nMensaje: Error DBA1\n"+ Connector->getLastError().text());
            }
            else
            {
                // Vamos a crear el query para eliminar la entrada(s)
                QSqlQuery* DelQuery= new QSqlQuery (Connector->Connector);

                // Ejecutemos la eliminacion
             /*   if (!DelQuery->exec(QString("DELETE FROM actividad WHERE id='")+
                                    ui->IDInput->text()+QString("'")))
                {
                    QMessageBox::critical(0, "Error",
                    "No se ha podido eliminar el registro."
                    "<br><br>Mensaje: Error DBA1<br>"+
                    Connector->getLastError().text());
                }
                else
                {
                    // Reportamos
                    qDebug("It worked!");
                }

*/
                Connector->EndConnection();
            }
        }
    }
}

/* Esta funcion marcara toda la fila entera al hacer click en una de las entradas.
 * Se debe hacer asi, probablemente porque QTableWidget no provee estos metodos
 * mientras que QTableView si. */
void LogReporter::MarkRow(int row, int column)
{
    ui->EventsList->selectRow(row);
}
