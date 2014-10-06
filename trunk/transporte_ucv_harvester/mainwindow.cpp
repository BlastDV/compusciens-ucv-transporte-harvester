#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Aunque quiza no sea del todo correcto, vamos a instanciar esto
     * aca puesto que necesitaremos sus datos durante la ejecucion de todo
     * el programa. */
    DevConnector= new DeviceConnector();
    connect(DevConnector, SIGNAL(RegistrarEvento(QString)), this, SLOT(ReportarMensaje(QString)));
    connect(DevConnector, SIGNAL(ReadingCodes(int)), this, SLOT(GetReadingUpdate(int)));
    //DevConnector->InitObject();

    ConnectionName= "MainWindow";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;

    LoadInitialData();

    /* Ahora establecemos las conexiones con los distintos objetos de la interfaz */
    connect(ui->CedulaInput, SIGNAL(textEdited(QString)), this, SLOT(UpdateTransportistaC(QString)));
    connect(ui->LastNameList, SIGNAL(currentIndexChanged(QString)), this, SLOT(UpdateTransportistaA(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}



// Esta funcion actualiza el usuario actual de la clase
bool MainWindow::UpdateUser(QString User)
{
    UserID= User;

    return true;
}

// Esto se ejecuta cuando el usuario presione "Conectar Dispositivo"
void MainWindow::on_actionConectarDispositivo_triggered()
{
    connect(DevConnector, SIGNAL(CancelPressed()), this, SLOT(DeviceConnectionAborted()));
    connect(DevConnector, SIGNAL(AcceptPressed()), this, SLOT(DeviceConnectionAccepted()));
    DevConnector->InitObject();
    DevConnector->show();
}

// Esta funcion se ejecuta si el usuario ejecuta y guarda la conexion al lector
void MainWindow::DeviceConnectionAccepted()
{
    disconnect(DevConnector, SIGNAL(AcceptPressed()), this, SLOT(DeviceConnectionAccepted()));
    DevConnector->hide();
}

// Esta funcion se ejecuta si el usuario cancela la conexion al lector
void MainWindow::DeviceConnectionAborted()
{
    disconnect(DevConnector, SIGNAL(CancelPressed()), this, SLOT(DeviceConnectionAborted()));
    DevConnector->hide();
}

/* Esta funcion enviara cualquier mensaje de sus clases hijas a la clase
 * padre appcontroller */
void MainWindow::ReportarMensaje(QString mensaje)
{
    emit ReportarAccion(mensaje);
}

/* Esta funcion preparara la interfaz de subida de datos con valores por defecto,
 * haciendo consultas en la BD y llenando ciertos campos necesarios para facilitar
 * esta tarea al usuario */
void MainWindow::LoadInitialData()
{
    ui->CodesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    //ui->AllCodesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->ReadingProgressBar->hide();

    // Bloqueamos la seccion de codigos
    ui->CodesFrame->setEnabled(false);

    ui->centralWidget->setStyleSheet("");
    ui->RouteDate->setDate(QDate::currentDate());
    ui->RouteHLlegada->setTime(QTime::currentTime());

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
        QSqlQuery* Lastnamequery= new QSqlQuery (Connector->Connector);

        if(!Lastnamequery->exec(QString("SELECT primer_apellido, cedula FROM transportista")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
            "No se ha podido recuperar la lista de transportistas. "
            "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ1\n");
        }
        else
        {
            Lastnamequery->first();

            // Ahora con los datos obtenidos del SELECT, vamos a llenar la interfaz

            // Llenamos la lista de apellidos
            do
            {
                ui->LastNameList->addItem(Lastnamequery->value(0).toString());
                CedulaList.append(Lastnamequery->value(1).toString());
            }
            while (Lastnamequery->next());

            // Ahora llenemos la lista de rutass
            QSqlQuery* Routequery= new QSqlQuery (Connector->Connector);

            if (!Routequery->exec(QString("SELECT * FROM ruta")))
            {
                QMessageBox::critical(0, QObject::tr("Error"),
                "No se ha podido recuperar la lista de rutas. "
                "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ2\n");
            }
            else
            {
                Routequery->first();

                // Guardemos el ID de la primera ruta
                int FirstRoute= Routequery->value(2).toInt();

                // Ahora con los datos obtenidos del SELECT, vamos a llenar la interfaz

                // Llenamos la lista de rutas
                do
                {
                    QString NombreRuta= QString(Routequery->value(0).toString()) + QString(" - ")
                            +QString(Routequery->value(1).toString());
                    ui->RouteList->addItem(NombreRuta);

                    // Y el "vector" de rutas
                    RouteLocalList[NombreRuta]= Routequery->value(2).toInt();

                }
                while (Routequery->next());

                // Ahora llenamos la lista de paradas segun la primera ruta
                QSqlQuery* Stopquery= new QSqlQuery (Connector->Connector);

                if (!Stopquery->exec(QString("SELECT * FROM parada WHERE ruta_id= %1").arg(FirstRoute)))
                {
                    QMessageBox::critical(0, QObject::tr("Error"),
                    "No se ha podido recuperar la lista de paradas. "
                    "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ3\n");
                }
                else
                {
                    Stopquery->first();

                    // Ahora con los datos obtenidos del SELECT, vamos a llenar la interfaz

                    // Llenamos la lista de paradas
                    do
                    {
                        ui->RouteStopsList->addItem(QString(Stopquery->value(1).toString()));
                    }
                    while (Stopquery->next());
                }
            }
        }

        Connector->EndConnection();
    }
}

/* Las siguientes funciones sirven para actualizar la interfaz segun el usuario
 * lo requiera. Otro transportista, otra ruta, etc. */
void MainWindow::UpdateTransportistaC(QString cedula)
{
    int id= cedula.toInt();

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
        QSqlQuery* Driverquery= new QSqlQuery (Connector->Connector);

        if (!Driverquery->exec(QString("SELECT * FROM transportista WHERE cedula=%1").arg(id)))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
            "No se ha podido recuperar la lista de transportistas. "
            "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ1\n");
        }
        else
        {
            Driverquery->first();

            if (Driverquery->isValid())
            {
                ui->Nombre1Input->setText(Driverquery->value(0).toString());
                ui->Nombre2Input->setText(Driverquery->value(1).toString());
                ui->Apellido2Input->setText(Driverquery->value(3).toString());
                ui->LastNameList->setCurrentIndex(ui->LastNameList->findText(Driverquery->value(2).toString()));
            }
            else
            {
                ui->Nombre1Input->setText("");
                ui->Nombre2Input->setText("");
                ui->Apellido2Input->setText("");
                ui->LastNameList->setCurrentIndex(0);
            }
        }
    }
}

void MainWindow::UpdateTransportistaA(QString apellido)
{
    if (ui->LastNameList->currentIndex()==0) // "Buscar por apellido"
    {
        ui->Nombre1Input->setText("");
        ui->Nombre2Input->setText("");
        ui->LastNameList->setCurrentIndex(0);
        ui->Apellido2Input->setText("");
        ui->CedulaInput->setText("");
    }
    else
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
            QSqlQuery* Driverquery= new QSqlQuery (Connector->Connector);

            if (!Driverquery->exec(QString("SELECT * FROM transportista WHERE primer_apellido=") +
                                   QString("'")+apellido+QString("'")))
            {
                QMessageBox::critical(0, QObject::tr("Error"),
                "No se ha podido recuperar la lista de transportistas. "
                "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ1\n");
            }
            else
            {
                Driverquery->first();

                if (Driverquery->isValid())
                {
                    ui->Nombre1Input->setText(Driverquery->value(0).toString());
                    ui->Nombre2Input->setText(Driverquery->value(1).toString());
                    ui->Apellido2Input->setText(Driverquery->value(3).toString());
                    ui->CedulaInput->setText(Driverquery->value(5).toString());
                }
            }
        }
    }
}

// Esto actualiza las paradas de la ruta escogida
void MainWindow::UpdateRoute(QString id)
{
    // Vamos a crear una conexion para poder traer ciertos datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
        "No se ha podido recuperar la lista de rutas. "
        "Revise el estado de la Base de Datos.\n\nMensaje: Error DBA1\n"+
        Connector->getLastError().text());
    }
    else
    {
        // Vamos a tomar el ID de la ruta que han seleccionado
        int RouteID= RouteLocalList.value(id);

        QSqlQuery* Stopquery= new QSqlQuery (Connector->Connector);

        if (!Stopquery->exec(QString("SELECT * FROM parada WHERE ruta_id= %1").arg(RouteID)))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
            "No se ha podido recuperar la lista de paradas. "
            "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ3\n");
        }
        else
        {
            Stopquery->first();
            ui->RouteStopsList->clear();

            // Ahora con los datos obtenidos del SELECT, vamos a llenar la interfaz

            // Llenamos la lista de paradas
            if (Stopquery->isValid())
            {
                do
                {
                    ui->RouteStopsList->addItem(QString(Stopquery->value(1).toString()));
                }
                while (Stopquery->next());
            }
        }
    }
}

// Esto actualiza las paradas de la ruta escogida en la pesta�a actual
void MainWindow::UpdateTabRoute(int RouteIndex)
{
    qDebug("RouteIndex: %d", RouteIndex);
    // Vamos a obtener el index de la pesta�a actual
    int TabIndex= ui->TripWindow->currentIndex();
    // Ahora a obtener el apuntador para la lista de paradas de la pesta�a actual
    QComboBox* AuxStops= TabsPointerList.at(TabIndex).Stops;

    // Ahora limpiamos la lista de paradas vieja y la llenamos con las de la ruta actual
    AuxStops->clear();
    for (int i=1; i<TripRutas.at(RouteIndex).count(); i++)
        AuxStops->addItem(TripRutas.at(RouteIndex).at(i));

    // Done
}

/* Esta funcion permitira extraer los codigos del lector para luego poder trabajar
 * con los mismos. La idea es que este proceso se ejecute una sola vez por lector,
 * aunque podria ser ejecutado multiples veces.
 */
void MainWindow::ReadCodes(bool automatico)
{
    /* Preparamos y mostramos un dialogo de progreso */
    QProgressDialog ReadingProgress;
    ReadingProgress.setWindowTitle("Recuperando c�digos...");
    ReadingProgress.setWindowModality(Qt::WindowModal);
    ReadingProgress.setMaximum(100);
    ReadingProgress.setMinimum(0);
    ReadingProgress.setValue(0);
    ReadingProgress.setCancelButton(0);
    connect(DevConnector, SIGNAL(ReadingCodes(int)), &ReadingProgress, SLOT(setValue(int)));
    ReadingProgress.show();
/*
    // Preparamos y mostramos la barra de carga
    ui->ReadingProgressBar->setValue(0);
    ui->ReadingProgressBar->show();

    // Bloqueamos los botones para evitar errores de comunicacion
    ui->CodesButtonsFrame->setEnabled(false);

    // Informamos al usuario sobre lo que se esta ejecutando actualmente
    ui->InfoText->clear();
    ui->InfoText->appendPlainText("Leyendo...");
*/
    int OpResult= DevConnector->Reader->cspReadData();

    if (OpResult> STATUS_OK)
    {
        // Informamos
        ui->InfoText->appendPlainText(QString("%1 c�digos recuperados").arg(OpResult));

        /* Vamos a crear una pesta�a que muestre todos los codigos leidos en una tabla */
        /* Primero creemos la tabla*/
        QTableWidget* AllCodesTable= new QTableWidget(0, 1, this);
        AllCodesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

        // Luego a�adimos la columna "En el dispositivo"
        QTableWidgetItem *Header = new QTableWidgetItem();
        Header->setText("En el dispositivo");
        AllCodesTable->setHorizontalHeaderItem(0, Header);

        // Crearemos un texto informativo
        QLabel* AllCodesInfo= new QLabel;
        AllCodesInfo->setText("Esta es la lista de todos los c�digos presentes en el dispositivo.");
        AllCodesInfo->setWordWrap(true);

        // Ahora creamos un layout e insertamos la tabla y el texto ahi
        QHBoxLayout *HLayout = new QHBoxLayout;
        HLayout->addWidget(AllCodesTable);
        HLayout->addWidget(AllCodesInfo);

        // Creamos un nuevo Widget que insertaremos como nueva pesta�a
        // Y le ponemos como Layout al que hemos creado arriba
        QWidget* AllCodesTab= new QWidget;
        AllCodesTab->setLayout(HLayout);

        // Insertamos el widget como nueva pesta�a
        ui->TripWindow->insertTab(0, AllCodesTab, "Todos los C�digos");

        QString LastCode="";
        int IgnoredCount=0;
        for (int i=0; i<OpResult; i++)
        {
            // Primero obtenemos el codigo
            QTableWidgetItem *Codigo;
            char Code[80];

            // Recuperamos el codigo en "Code"
            DevConnector->Reader->cspGetBarcode(Code, i, sizeof(Code));

            // Luego lo limpiamos
            QString AuxCode= QString(Code);
            if (AuxCode.at(0)=='B')
                AuxCode= AuxCode.remove('B');

            // Y si no es igual al anterior (errores de usuario)
            if (AuxCode!=LastCode)
            {
                // Lo insertamos
                AllCodesTable->insertRow(AllCodesTable->rowCount());
                AllCodesTable->setRowHeight(AllCodesTable->rowCount()-1, 20);

                Codigo= new QTableWidgetItem (AuxCode);
                Codigo->setFlags(Codigo->flags() ^ Qt::ItemIsEditable); // No editable

                // Y lo insertamos en la tabla de todos los codigos
                AllCodesTable->setItem(AllCodesTable->rowCount()-1, 0, Codigo);

                // Finalmente insertamos el codigo en la lista global
                CodesList.append(AuxCode);

                LastCode= AuxCode;
            }
            else
            {
                IgnoredCount++;
            }
        }
        // Informamos
        ui->InfoText->appendPlainText(QString("%1 c�digos ignorados").arg(IgnoredCount));

        /* Llegados a este punto, toca crear la lista de pasajeros por viajes de acuerdo
         * al analisis de la lista de codigos. El algoritmo tendra dos versiones: aquella
         * en la que el usuario ha especificado explicitamente cual es el transportista activo
         * y aquella en la que ha decidido que el sistema lo determine automaticamente. Para este
         * ultimo caso, si se detecta mas de un transportista en la lista, el sistema
         * preguntara al usuario cual de ellos es el actual.
         */
        if (automatico)
            CalculateTrips();
        else
            CalculateTrips(ui->CedulaInput->text());
    }
    else
    {
        // Informamos
        ui->InfoText->appendPlainText("No se han podido recuperar los c�digos. �El lector est� conectado?");
    }

    // Finalmente, rehabilitamos la interfaz y escondemos la barra de carga
    ui->ReadingProgressBar->setValue(100);
    ui->ReadingProgressBar->hide();
    ui->CodesButtonsFrame->setEnabled(true);
}

// Funciones de apoyo para ReadCodes()
void MainWindow::CalculateTrips()
{
    // Esto nos permitira saber las coincidencias de transportistas en
    // la lista de codigos escaneados
    QList <QString> FoundDrivers;

   // for (int i=0; i<CedulaList.count(); i++)
     //   qDebug("Cedula: %s", CedulaList.at(i).toStdString().c_str());

    // Vamos a comparar cada codigo con cada cedula de los transportistas
    for (int i=0; i<CedulaList.count(); i++)
    {
        int startj= 0;
        for (int j=startj; j<CodesList.count(); j++)
        {
            // Hay coincidencia?
            if (CodesList.at(j)==CedulaList.at(i))
            {
                FoundDrivers.append(CedulaList.at(i));
                // Seguiremos buscando desde aca, pero no tiene sentido
                // seguir preguntando por el conductor actual asi que
                // rompemos y pasamos al siguiente
                startj= j;
                break;
            }
        }
    }

    // Una vez que hemos revisado las coincidencias, vamos a mostrarselas
    // al usuario

    if (!FoundDrivers.isEmpty())
    {
        /* Vamos a recuperar la informacion de los transportistas desde
         * la base de datos
        */

        // Vamos a crear una conexion
        if (!Connector->RequestConnection())
        {
            QMessageBox::critical(0, "Error",
            "No se ha podido recuperar la informaci�n de los transportistas encontrados.\n"
            "Revise el estado de la Base de Datos.\n\nMensaje: Error DBA1\n"+
            Connector->getLastError().text());
        }
        else
        {
            QStringList Drivers;
            QSqlQuery* Driverquery= new QSqlQuery (Connector->Connector);

            // Preparemos la lista de resultadoss
            for (int k=0; k<FoundDrivers.count(); k++)
            {
                int id= FoundDrivers.at(k).toInt();

                if (!Driverquery->exec(QString("SELECT primer_apellido, segundo_apellido, primer_nombre, segundo_nombre FROM transportista WHERE cedula=%1").arg(id)))
                {
                    QMessageBox::critical(0, "Error",
                    "No se ha podido recuperar la informaci�n de los transportistas encontrados.\n"
                    "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ1\n");
                }
                else
                {
                    Driverquery->first();

                    if (Driverquery->isValid())
                    {
                        Drivers.append(FoundDrivers.at(k) + ", " + Driverquery->value(0).toString() + " " + Driverquery->value(1).toString()
                                       + " " + Driverquery->value(2).toString() + " " + Driverquery->value(3).toString());
                    }
                    else
                    {
                        Drivers.append(FoundDrivers.at(k) + ", error de base de datos.");
                    }
                }
            }

            // Ahora preparemos y mostremos la ventana de resultados
            bool OK;
            QString PickedDriver= QInputDialog::getItem(this, "Transportistas encontrados",
                                                        "Seleccione uno de los siguientes transportistas:",
                                                        Drivers, 0, false, &OK);

            // Si el usuario elige un transportista
            if (OK)
            {
                // Vamos a confirmar el resto del procedimiento
                QString Cedula= PickedDriver.mid(0, PickedDriver.indexOf(",", 0, Qt::CaseInsensitive));

                // Actualizamos el transportista actual
                UpdateTransportistaC(Cedula);

                // Calculamos las rutas
                CalculateTrips(Cedula);
            }
            else
            {
                // Ignore
            }
        }
    }
    else
    {
        QMessageBox::information(0, "Resultados de la busqueda",
        "No se han encontrado transportistas.\n\n"
        "�El lector pertenece a un transportista nuevo?\n"
        "�El transportista uso correctamente el lector?");
    }
}

// Este es uno de los procedimientos mas importantes del sistema, permitira calcular los viajes de acuerdo
// a la cedula del transportista actual y los ubicara en una serie de pesta�as para su posterior configuracion
// de rutas, hora de salida, llegada y fecha.
void MainWindow::CalculateTrips(QString Cedula)
{
    /** Empezamos a llenar los viajes, para que el usuario pueda asociarlos con las rutas **/

    // En primer lugar, vamos a manejar las mismas rutas para todos los viajes, asi que las consultaremos una
    // vez y haremos uso de ellas cada vez que sea necesario

    // Vamos a crear una conexion para poder traer los datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
        "No se ha podido recuperar la lista de transportistas. "
        "Revise el estado de la Base de Datos.\n\nMensaje: Error DBA1\n"+
        Connector->getLastError().text());
    }
    else
    {
        // Si todo va bien en la conexion, recuperaremos los datos y mostraremos las pesta�as
        QSqlQuery* Routequery= new QSqlQuery (Connector->Connector);

        if (!Routequery->exec(QString("SELECT * FROM ruta")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
            "No se ha podido recuperar la lista de rutas. "
            "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ2\n");
        }
        else
        {
            Routequery->first();
            // Llenamos la lista de rutas
            do
            {
                int CurrentRouteID= Routequery->value(2).toInt();

                QString NombreRuta= Routequery->value(0).toString() + "-"
                        +Routequery->value(1).toString()+"-"+Routequery->value(2).toString();
                ui->RouteList->addItem(NombreRuta);

                QStringList AuxRoute;
                AuxRoute.append(NombreRuta);

                // Ahora llenamos la lista de paradas segun la ruta actual
                QSqlQuery* Stopquery= new QSqlQuery (Connector->Connector);

                if (!Stopquery->exec(QString("SELECT * FROM parada WHERE ruta_id= %1").arg(CurrentRouteID)))
                {
                    QMessageBox::critical(0, QObject::tr("Error"),
                    "No se ha podido recuperar la lista de paradas. "
                    "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ3\n");
                }
                else
                {
                    Stopquery->first();
                    // Llenamos la lista de paradas
                    do
                    {
                        AuxRoute.append(Stopquery->value(1).toString());
                    }
                    while (Stopquery->next());
                }

                // Ahora insertamos el QStringList en el vector de rutas
                TripRutas.append(AuxRoute);
            }
            while (Routequery->next());
        }

        Connector->EndConnection();
    }

    int ViajesCount= 0;
    for (int i= 0; i<CodesList.count(); i++)
    {
        // Si hemos encontrado la cedula del transportista, debemos crear una nueva pesta�a para un nuevo viaje
        // Tambien aplica si es la primera iteracion del ciclo, para tener un viaje inicial
        if (CodesList.at(i)==Cedula || i==0)
        {
            ViajesCount++;
            // Creamos la pesta�a "Viaje 1"
            /* Primero creemos la tabla*/
            QTableWidget* TripTable= new QTableWidget(0, 1, this);
            TripTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

            // Luego a�adimos la columna "Pasajeros"
            QTableWidgetItem *Header = new QTableWidgetItem();
            Header->setText("Pasajeros");
            TripTable->setHorizontalHeaderItem(0, Header);

            // Ahora, crearemos el widget derecho que tendra la asociacion de las rutas

            // Los <inputs>
            QComboBox* RouteList= new QComboBox;
            QComboBox* StopsList= new QComboBox;
            QTimeEdit* DepartTInput= new QTimeEdit;
            QTimeEdit* ArriveTInput= new QTimeEdit;
            QDateEdit* DateInput= new QDateEdit;
            ArriveTInput->setTime(QTime::currentTime());
            DateInput->setDate(QDate::currentDate());
            DateInput->setCalendarPopup(true);

            // Ahora llenemos estos inputs
            for (int u=0; u<TripRutas.count(); u++)
                RouteList->addItem(TripRutas.at(u).at(0));
            RouteList->setCurrentIndex(RouteList->count()-1);
            for (int u=1; u<TripRutas.at(TripRutas.count()-1).count(); u++)
                StopsList->addItem(TripRutas.at(TripRutas.count()-1).at(u));

            // Y hacemos la conexion para el cambio de rutas
            connect(RouteList, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateTabRoute(int)));

            // El Layout
            QFormLayout* RoutesLayout= new QFormLayout;
            RoutesLayout->addRow("Ruta:", RouteList);
            RoutesLayout->addRow("Paradas:", StopsList);
            RoutesLayout->addRow("Hora de Salida:", DepartTInput);
            RoutesLayout->addRow("Hora de Llegada:", ArriveTInput);
            RoutesLayout->addRow("Fecha:", DateInput);

            // El groupBox
            QGroupBox* RoutesBox= new QGroupBox("Datos de la Ruta");

            // Insertamos el Layout al GroupBox
            RoutesBox->setLayout(RoutesLayout);

            // Ahora creamos un layout horizontal e insertamos la tabla y el groupbox ahi
            QHBoxLayout *HLayout = new QHBoxLayout;
            HLayout->addWidget(TripTable);
            HLayout->addWidget(RoutesBox);

            // Creamos un nuevo Widget que insertaremos como nueva pesta�a
            // Y le ponemos como Layout al que hemos creado arriba
            QWidget* AllCodesTab= new QWidget;
            AllCodesTab->setLayout(HLayout);

            // Insertamos el widget como nueva pesta�a
            ui->TripWindow->insertTab(ui->TripWindow->count()-2, AllCodesTab, QString("Viaje %1").arg(ViajesCount));

            // Finalmente, guardamos todas las referencias relevantes
            trip_tab AuxTab;
            AuxTab.RouteName= RouteList;
            AuxTab.Stops= StopsList;
            AuxTab.DepartTime= DepartTInput;
            AuxTab.ArriveTime= ArriveTInput;
            AuxTab.Date= DateInput;
            AuxTab.CodesTable= TripTable;
            TabsPointerList.append(AuxTab);
        }
        else
        {
            // Si no, seguimos insertando en la lista actual
            // Creamos la fila
            QTableWidget* CurrentTable= TabsPointerList.at(TabsPointerList.count()-1).CodesTable;
            CurrentTable->insertRow(CurrentTable->rowCount());
            CurrentTable->setRowHeight(CurrentTable->rowCount()-1, 20);

            // E insertamos el codigo
            QTableWidgetItem* AuxTableItem= new QTableWidgetItem (CodesList.at(i));
            CurrentTable->setItem(CurrentTable->rowCount()-1, 0, AuxTableItem);
        }

    }
}

// Esto recibe la actualizacion del proceso de lectura de DeviceConnector
void MainWindow::GetReadingUpdate(int)
{
    if (ui->ReadingProgressBar->value()<90)
        ui->ReadingProgressBar->setValue(ui->ReadingProgressBar->value()+5);
}

/* Esto servira para traernos los codigos del lector */
void MainWindow::on_CalculateTripsButton_clicked()
{
    ReadCodes(false);
}

/* Esto hara lo mismo que on_CalculateTripsButton_clicked() pero
 * buscara automaticamente al transportista */
void MainWindow::on_FindTransportistButton_clicked()
{
    ReadCodes(true);
    ui->DriversFrame->setEnabled(false);
    ui->CodesFrame->setEnabled(true);
}

/* Esto se ejecutara cuando el usuario haya elegido al transportista correspondiente
 * al lector actual */
void MainWindow::on_DriverReadyButton_clicked()
{
    // Bloqueamos la seccion de transportistas
    ui->DriversFrame->setEnabled(false);

    // Y desbloqueamos la de codigos
    ui->CodesFrame->setEnabled(true);
}

// Esto se activara cuando el usuario eliga volver a la eleccion de transportista
void MainWindow::on_BackToDriverButton_clicked()
{
    // Bloqueamos la seccion de codigos
    ui->CodesFrame->setEnabled(false);

    // La limpiamos
    ui->TripWindow->clear();

    // Y desbloqueamos la de transportista
    ui->DriversFrame->setEnabled(true);
}

// Esto se ejecutara cuando el usuario haga click en Administrar base de datos/Transportistas
void MainWindow::on_actionAdminTransportistas_triggered()
{
    // Creamos el objeto
    DriversManager* DriversM= new DriversManager();

    DriversM->UpdateUser(UserID);
    DriversM->show();
}

// Cuando se haga click en "Cerrar Sesion", se emitira una se�al a SessionWindow para que haga lo propio
void MainWindow::on_actionCerrarSesion_triggered()
{
    emit CerrarSesion();
}

// Cuando se haga click en "Ver el registro de Sucesos", se ejecutara lo siguiente que consiste en crear el objeto LogReporter
// y mostrarlo, para posteriormente destruirlo
void MainWindow::on_actionVerLog_triggered()
{
    LogRep= new LogReporter ();

    LogRep->UpdateUser(UserID);
    LogRep->show();
}

void MainWindow::on_actionAdminUsuarios_triggered()
{
    // Creamos el objeto
    UsersManager* UsersM= new UsersManager();

    UsersM->UpdateUser(UserID);
    UsersM->show();
}
