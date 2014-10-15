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
    DevConnector->InitObject();

    ConnectionName= "MainWindow";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;

    // Esto es para saber si hay un transportista escogido o no
    DRIVERCHOSEN= false;

    // Cargamos todos los datos iniciales de la clase
    LoadInitialData();

    /* Escondemos la barra de carga de codigos */
    ui->ReadingProgressBar->hide();

    // Activamos REGEX en los inputs
    ActivateValidators();

    /* Ahora establecemos las conexiones con los distintos objetos de la interfaz */
    connect(ui->CedulaInput, SIGNAL(textEdited(QString)), this, SLOT(UpdateTransportistaC(QString)));
    connect(ui->LastNameList, SIGNAL(currentIndexChanged(QString)), this, SLOT(UpdateTransportistaA(QString)));

    /* Finalmente ocultemos la interfaz hasta que el usuario no conecte un lector */
    ui->MainFrame->hide();
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
    DevConnector->show();
}

// Esta funcion se ejecuta si el usuario ejecuta y guarda la conexion al lector
void MainWindow::DeviceConnectionAccepted()
{
    disconnect(DevConnector, SIGNAL(AcceptPressed()), this, SLOT(DeviceConnectionAccepted()));
    DevConnector->hide();

    // Ahora mostremos la interfaz
    ui->MainFrame->show();
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

// Esta funcion evita que el usuario ingrese simbolos o espacios en el campo de cedula de transportista
void MainWindow::ActivateValidators()
{
    // Creemos las validaciones de la cedula
    QRegExp IDReg("([0-9]){5,9}");
    QValidator *IDValidator= new QRegExpValidator(IDReg, this);

    ui->CedulaInput->setValidator(IDValidator);
}

/* Esta funcion preparara la interfaz de subida de datos con valores por defecto,
 * haciendo consultas en la BD y llenando ciertos campos necesarios para facilitar
 * esta tarea al usuario */
void MainWindow::LoadInitialData()
{
    // Borramos las estructuras de la clase
    WipeTripsOut();

    // Bloqueamos la seccion de codigos
    ui->CodesFrame->setEnabled(false);

    ui->centralWidget->setStyleSheet("");

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
        }

        // Ahora hay que cargar la informacion de las rutas
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
            if (Routequery->isValid())
            {
                // Llenamos la lista de rutas
                do
                {
                    int CurrentRouteID= Routequery->value(2).toInt();

                    QString NombreRuta= Routequery->value(0).toString() + "-"
                            +Routequery->value(1).toString()+"-"+Routequery->value(2).toString();
                    //ui->RouteList->addItem(NombreRuta);

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
                        if (Stopquery->isValid())
                        {
                            // Llenamos la lista de paradas
                            do
                            {
                                AuxRoute.append(Stopquery->value(1).toString());
                            }
                            while (Stopquery->next());
                        }
                    }

                    // Ahora insertamos el QStringList en el vector de rutas
                    TripRutas.append(AuxRoute);
                }
                while (Routequery->next());
            }
        }

        Connector->EndConnection();
    }
}

/* Esto permite limpiar las estructuras de la clase, ademas de vaciar el widget de viajes y dejar
 * todo listo para una nueva lectura. Deberia ser llamado al subir los datos a la BD, al cambiar el
 * transportista para el lector actual o al cambiar de lector para registrar el trabajo de varios
 * transportistas */
void MainWindow::WipeTripsOut()
{
    // Vamos a vaciar la tabla de todos los codigos y todos sus apuntadores
    TabsPointerList.clear();
    ui->TripWindow->clear();

    // Ademas de la lista de codigos por pestañas
    FinalCodesList.clear();
}

/* Las siguientes funciones sirven para actualizar la interfaz segun el usuario
 * lo requiera. Otro transportista, otra ruta, etc. */
// Esta en particular actualiza los datos segun la cedula ingresada y ademas chequea
// que se trate de un transportista valido, para evitar usar el lector sin un transportista
// previamente elegido
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

                DRIVERCHOSEN= true;
                ui->DriverReadyButton->setEnabled(true);
            }
            else
            {
                ui->Nombre1Input->setText("");
                ui->Nombre2Input->setText("");
                ui->Apellido2Input->setText("");
                ui->LastNameList->setCurrentIndex(0);

                DRIVERCHOSEN= false;
                ui->DriverReadyButton->setEnabled(false);
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
        //ui->CedulaInput->setText("");

        DRIVERCHOSEN= false;
        ui->DriverReadyButton->setEnabled(false);
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

                    // Permitimos el pase a la otra etapa del proceso de carga
                    DRIVERCHOSEN= true;
                    ui->DriverReadyButton->setEnabled(true);
                }
                else
                {
                    // Denegamos el pase
                    DRIVERCHOSEN= false;
                    ui->DriverReadyButton->setEnabled(false);
                }
            }
        }
    }
}

// Esto actualiza las paradas de la ruta escogida en la pestaña actual
void MainWindow::UpdateTabRoute(int RouteIndex)
{
    // Vamos a obtener el index de la pestaña actual
    int TabIndex= ui->TripWindow->currentIndex();

    // Ahora a obtener el apuntador para la lista de paradas de la pestaña actual
    QComboBox* AuxStops= TabsPointerList.at(TabIndex+1).Stops;

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
    /* Borremos cualquier codigo previamente leido */
    CodesList.clear();

    /* Mostramos la barra de progreso y otros avisos */
    ui->ReadingProgressBar->show();
    ui->BackToDriverButton->hide();
    ui->InfoText->clear();
    ui->InfoText->insertPlainText("Leyendo códigos, por favor espere ...");

    int OpResult= DevConnector->Reader->cspReadData();

    if (OpResult> STATUS_OK)
    {
        // Informamos
        ui->InfoText->appendPlainText(QString("%1 códigos recuperados").arg(OpResult));

        /* Vamos a crear una pestaña que muestre todos los codigos leidos en una tabla */
        /* Primero creemos la tabla*/
        QTableWidget* AllCodesTable= new QTableWidget(0, 1, this);
        AllCodesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

        // Luego añadimos la columna "En el dispositivo"
        QTableWidgetItem *Header = new QTableWidgetItem();
        Header->setText("En el dispositivo");
        AllCodesTable->setHorizontalHeaderItem(0, Header);

        // Crearemos un texto informativo
        QLabel* AllCodesInfo= new QLabel;
        AllCodesInfo->setText("Esta es la lista de todos los códigos presentes en el dispositivo.");
        AllCodesInfo->setWordWrap(true);

        // Ahora creamos un layout e insertamos la tabla y el texto ahi
        QHBoxLayout *HLayout = new QHBoxLayout;
        HLayout->addWidget(AllCodesTable);
        HLayout->addWidget(AllCodesInfo);

        // Creamos un nuevo Widget que insertaremos como nueva pestaña
        // Y le ponemos como Layout al que hemos creado arriba
        QWidget* AllCodesTab= new QWidget;
        AllCodesTab->setLayout(HLayout);

        // Insertamos el widget como nueva pestaña
        ui->TripWindow->insertTab(0, AllCodesTab, "Todos los Códigos");

        // Ahora toca salvar el apuntador a esa tabla para poder borrarla
        // y/o manipularla mas adelante
        trip_tab AuxTab;
        AuxTab.TabPointer= AllCodesTab;
        TabsPointerList.append(AuxTab);

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
        ui->InfoText->appendPlainText(QString("Códigos repetidos ignorados: %1").arg(IgnoredCount));

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

        // Si todo ha ido bien, habilitamos los botones de accion
        ui->UploadDataButton->setEnabled(true);
        ui->NextDeviceButton->setEnabled(true);

        // Y la seccion de codigos
        ui->CodesButtonsFrame->setEnabled(true);
    }
    else
    {
        // Informamos
        ui->InfoText->appendPlainText("No se han podido recuperar los códigos. ¿El lector está conectado?");

        // Y volvemos al paso anterior
        on_BackToDriverButton_clicked();
    }

    // Finalmente, rehabilitamos la interfaz y escondemos la barra de carga
    ui->ReadingProgressBar->hide();
    ui->BackToDriverButton->show();
}

// Funciones de apoyo para ReadCodes()
void MainWindow::CalculateTrips()
{
    // Esto nos permitira saber las coincidencias de transportistas en
    // la lista de codigos escaneados
    QList <QString> FoundDrivers;

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
            "No se ha podido recuperar la información de los transportistas encontrados.\n"
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
                    "No se ha podido recuperar la información de los transportistas encontrados.\n"
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

            Connector->EndConnection();
        }
    }
    else
    {
        QMessageBox::information(0, "Resultados de la busqueda",
        "No se han encontrado transportistas.\n\n"
        "¿El lector pertenece a un transportista nuevo?\n"
        "¿El transportista uso correctamente el lector?");
    }
}

// Este es uno de los procedimientos mas importantes del sistema, permitira calcular los viajes de acuerdo
// a la cedula del transportista actual y los ubicara en una serie de pestañas para su posterior configuracion
// de rutas, hora de salida, llegada y fecha.
void MainWindow::CalculateTrips(QString Cedula)
{
    /** Empezamos a llenar los viajes, para que el usuario pueda asociarlos con las rutas **/

    // En primer lugar, vamos a manejar las mismas rutas para todos los viajes, asi que las consultaremos una
    // vez y haremos uso de ellas cada vez que sea necesario

    int ViajesCount= 0;
    int IgnoredCodes= 0; // Esto permitira reportar cuantas repeticiones por viajes fueron ignoradas
    for (int i= 0; i<CodesList.count(); i++)
    {
        // Si hemos encontrado la cedula del transportista, debemos crear una nueva pestaña para un nuevo viaje
        // Tambien aplica si es la primera iteracion del ciclo, para tener un viaje inicial
        if (CodesList.at(i)==Cedula || i==0)
        {
            ViajesCount++;

            // Primero vamos a crear un vector de codigos para la pestaña actual
            QVector <QString> CurrentTabCodes;
            FinalCodesList.append(CurrentTabCodes);

            /* Creemos la tabla*/
            QTableWidget* TripTable= new QTableWidget(0, 1, this);
            TripTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

            // Luego añadimos la columna "Pasajeros"
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

            // Creamos un nuevo Widget que insertaremos como nueva pestaña
            // Y le ponemos como Layout al que hemos creado arriba
            QWidget* NewTab= new QWidget;
            NewTab->setLayout(HLayout);

            // Insertamos el widget como nueva pestaña
            ui->TripWindow->insertTab(ui->TripWindow->count()-1, NewTab, QString("Viaje %1").arg(ViajesCount));

            // Finalmente, guardamos todas las referencias relevantes
            trip_tab AuxTab;
            AuxTab.RouteName= RouteList;
            AuxTab.Stops= StopsList;
            AuxTab.DepartTime= DepartTInput;
            AuxTab.ArriveTime= ArriveTInput;
            AuxTab.Date= DateInput;
            AuxTab.CodesTable= TripTable;
            AuxTab.TabPointer= NewTab;
            TabsPointerList.append(AuxTab);

            // Con la estructura creada, toca insertar la primera cedula para no perderla, salvo
            // que la cedula corresponda al transportista o que ya haya sido insertada en la lista
            if (CodesList.at(i)!=Cedula)
            {
                // El codigo existe ya?
                if (!FinalCodesList.at(ViajesCount-1).contains(CodesList.at(i)))
                {
                    TripTable->insertRow(TripTable->rowCount());
                    TripTable->setRowHeight(TripTable->rowCount()-1, 20);

                    QTableWidgetItem* AuxTableItem= new QTableWidgetItem (CodesList.at(i));
                    TripTable->setItem(TripTable->rowCount()-1, 0, AuxTableItem);

                    // Insertamos en la lista de la pestaña actual
                    FinalCodesList[ViajesCount-1].append(CodesList.at(i));
                }
                else
                {
                    IgnoredCodes++;
                }
            }
        }
        else
        {
            // Si no, seguimos insertando en la lista actual siempre que el codigo actual no haya sido
            // insertado ya
            // El codigo existe ya?
            if (!FinalCodesList.at(ViajesCount-1).contains(CodesList.at(i)))
            {
                // Creamos la fila
                QTableWidget* CurrentTable= TabsPointerList.at(TabsPointerList.count()-1).CodesTable;
                CurrentTable->insertRow(CurrentTable->rowCount());
                CurrentTable->setRowHeight(CurrentTable->rowCount()-1, 20);

                // E insertamos el codigo
                QTableWidgetItem* AuxTableItem= new QTableWidgetItem (CodesList.at(i));
                CurrentTable->setItem(CurrentTable->rowCount()-1, 0, AuxTableItem);

                // Insertamos en la lista de la pestaña actual
                FinalCodesList[ViajesCount-1].append(CodesList.at(i));
            }
            else
            {
                IgnoredCodes++;
            }
        }
    }

    // Informamos
    ui->InfoText->appendPlainText(QString("Códigos repetidos por pestaña ignorados: %1").arg(IgnoredCodes));
}

/* Esto hara lo mismo que on_CalculateTripsButton_clicked() pero
 * buscara automaticamente al transportista */
void MainWindow::on_FindTransportistButton_clicked()
{
    ui->DriversFrame->setEnabled(false);
    ui->CodesFrame->setEnabled(true);
    ReadCodes(true);
}

/* Esto se ejecutara cuando el usuario haya elegido al transportista correspondiente
 * al lector actual */
void MainWindow::on_DriverReadyButton_clicked()
{
    // Bloqueamos la seccion de transportistas
    ui->DriversFrame->setEnabled(false);

    // Desbloqueamos la de codigos
    ui->CodesFrame->setEnabled(true);

    // Y leemos del dispositivo
    ReadCodes(false);
}

// Esto se activara cuando el usuario eliga volver a la eleccion de transportista
void MainWindow::on_BackToDriverButton_clicked()
{
    // Bloqueamos las acciones
    ui->UploadDataButton->setEnabled(false);
    ui->NextDeviceButton->setEnabled(false);

    // Bloqueamos la seccion de codigos
    ui->CodesFrame->setEnabled(false);

    // La limpiamos
    WipeTripsOut();

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

// Cuando se haga click en "Cerrar Sesion", se emitira una señal a SessionWindow para que haga lo propio
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

/** Esta funcion es la mas importante de todo el proyecto, es la que se encargara
 * de tomar la informacion capturada y procesada del dispositivo para subirla
 * a la base de datos, de forma que esta pueda ser consultada en la fase 2: Reporter */
void MainWindow::on_UploadDataButton_clicked()
{
    // Vamos a crear una conexion
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
        "No se ha podido recuperar la información de los transportistas encontrados.\n"
        "Revise el estado de la Base de Datos.\n\nMensaje: Error DBA1\n"+
        Connector->getLastError().text());
    }
    else
    {
        /* Necesitamos 3 insert:
         * Uno para la tabla "pasajero"
         * Uno para la tabla "viaje"
         * Uno para la tabla "registro"
         */
        QSqlQuery* PassengerQuery= new QSqlQuery(Connector->Connector);
        QSqlQuery* TripQuery= new QSqlQuery(Connector->Connector);
        QSqlQuery* RecordQuery= new QSqlQuery(Connector->Connector);

        // Ahora, por cada viaje, vamos a insertar un conjunto de pasajeros
        int TripCount= TabsPointerList.count()-1; // Restamos 1 para ignorar la pestaña "Todos los codigos"
        bool OK= true; // Para detener todo el proceso por si algo sale mal!
        for (int i=0; i<TripCount; i++)
        {
            // Primero hay que obtener el ID de ruta
            QStringList AuxToken= TabsPointerList.at(i+1).RouteName->currentText().split("-");
            QString Ruta_ID= AuxToken.at(AuxToken.count()-1);
            QString CurrentTrip_ID= ""; // Esto permitira saber el ID del viaje que estamos insertando (porque es "auto increment")
            QVector <QString> PASSENGERS_IDS; // Esto tambn permitira conocer los ID de los pasajeros insertados

            // Insertemos el viaje actual
            if (!TripQuery->exec(QString("INSERT INTO viaje (fecha, hora_salida, hora_llegada, ruta_id, ci_transportista) VALUES(")+
                                 QString("'")+TabsPointerList.at(i+1).Date->date().toString("yyyy-MM-dd")+QString("', ")+
                                 QString("'")+TabsPointerList.at(i+1).DepartTime->time().toString("HH:mm:ss")+QString("', ")+
                                 QString("'")+TabsPointerList.at(i+1).ArriveTime->time().toString("HH:mm:ss")+QString("', ")+
                                 QString("'")+Ruta_ID+QString("', ")+
                                 QString("'")+ui->CedulaInput->text()+QString("')")))
            {
                QMessageBox::information(0, "Error",
                "No se han podido guardar los viajes. Revise el estado de la base<br>"
                "de datos.<br><br>"
                "Mensaje: Error DBQ1<br>"+
                Connector->getLastError().text());

                // Toca matar el procedimiento si la insercion falla
                OK= false;
                break;
            }
            else
            {
                // Ahora recuperemos el ID del ultimo viaje
                if (!TripQuery->exec("SELECT LAST_INSERT_ID()"))
                {
                    QMessageBox::information(0, "Error",
                    "No se han podido guardar los viajes. Revise el estado de la base<br>"
                    "de datos.<br><br>"
                    "Mensaje: Error DBQ2<br>"+
                    Connector->getLastError().text());

                    // Toca matar el procedimiento si la recuperacion del ID falla
                    OK= false;
                    break;
                }
                else
                {
                    TripQuery->first();
                    CurrentTrip_ID= TripQuery->value(0).toString();
                }
            }


            // Insertemos cada pasajero
            for (int p=0; p<FinalCodesList.at(i).count(); p++)
            {
                if (!PassengerQuery->exec(QString("INSERT INTO pasajero (cedula) VALUES(")+
                                          QString("'")+FinalCodesList.at(i).at(p)+QString("')")))
                {
                    // Si a estas alturas no podemos insertar pasajeros, es que algo ha salido muy mal
                    QMessageBox::information(0, "Error",
                    "No se ha podido guardar la lista de pasajeros. Revise el estado de la base<br>"
                    "de datos.<br><br>"
                    "Mensaje: Error DBQ1<br>"+
                    Connector->getLastError().text());

                    // Por eso toca matar el procedimiento
                    OK= false;
                    break;
                }
                else
                {
                    // Ahora recuperemos el ID del ultimo pasajero insertado
                    if (!PassengerQuery->exec("SELECT LAST_INSERT_ID()"))
                    {
                        QMessageBox::information(0, "Error",
                        "No se ha podido guardar la lista de pasajeros. Revise el estado de la base<br>"
                        "de datos.<br><br>"
                        "Mensaje: Error DBQ2<br>"+
                        Connector->getLastError().text());

                        // Si fallo esto, toca matar el procedimiento
                        OK= false;
                        break;
                    }
                    else
                    {
                        PassengerQuery->first();
                        PASSENGERS_IDS.append(PassengerQuery->value(0).toString());
                    }
                }
            }

            // Todo salio bien?
            if (!OK)
                break;
            else
            {
                // Ahora solo queda guardar las entradas en la tabla "registro"

                for (int i=0; i<PASSENGERS_IDS.count(); i++)
                {
                    if (!RecordQuery->exec(QString("INSERT INTO registro (viaje_id, pasajero_id) VALUES (")+
                                           QString("'")+CurrentTrip_ID+QString("', ")+
                                           QString("'")+PASSENGERS_IDS.at(i)+QString("')")))
                    {
                        OK= false;
                        break;
                    }
                }
            }
        }

        if (!OK)
            QMessageBox::critical(0, "Error",
            "No se han podido registrar los viajes correctamente. Revise el estado de la base<br>"
            "de datos.<br><br>");
        else
        {
            QMessageBox::information(0, "Fin de Operacion",
            "¡Se han guardado los datos correctamente!<br><br>Puede desconectar el dispositivo e insertar otro si es necesario.<br>");

            // Limpiemos la interfaz y dejemosla como estaba
            on_NextDeviceButton_clicked();
        }
    }
}

// Esto basicamente lo que hace es proporcionar una forma explicita de eliminar
// los datos calculados de la interfaz (pestañas de viajes, etc).
void MainWindow::on_NextDeviceButton_clicked()
{
    // Que es exactamente lo mismo que hace el boton "Cambiar Transportista"
    on_BackToDriverButton_clicked();
}

// Esto llama a la ventana de configuracion del dispositivo
void MainWindow::on_actionConfigurarDispositivo_triggered()
{
    DevSetting= new DeviceSetting();
    DevSetting->show();
}
