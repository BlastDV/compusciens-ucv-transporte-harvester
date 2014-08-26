#include "driversmanager.h"
#include "ui_driversmanager.h"

DriversManager::DriversManager(QWidget *parent) : QWidget(parent), ui(new Ui::DriversManager)
{
    ui->setupUi(this);

    // Ningun usuario nos ve por ahora
    UserID= "default";

    // Este sera nuestro informante de permisos
    PermissionRep= new PermissionReporter(this);

    // Inicializamos el indice de registros
    RegIndex= -1;

    // No estamos editando
    EDITING= false;

    // Iniciamos el conector de la BD
    ConnectionName= "DriversManager";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;

    // Hagamos las conexiones entre signals y slots
    connect(ui->ApellidoSearchList, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateInputsA(int)));
    connect(ui->CedulaSearchInput, SIGNAL(textEdited(QString)), this, SLOT(UpdateInputsC(QString)));

    // Activemos las validaciones
    ActivateValidators();

    // Llenamos la interfaz de informacion
    LoadData();

    // Y la mostramos de forma preliminar, esto es: vacia. Por razones de seguridad
    // en posibles usos incorrectos de esta clase. Debe llamarse UpdateUser() antes de mostrarla!!!
    SetView("default");
}

DriversManager::~DriversManager()
{
    delete ui;
}

// Esta funcion se traera los datos de la cedula de entrada cuando se haga una busqueda
void DriversManager::UpdateInputsC(QString Cedula)
{
    int id= Cedula.toInt();

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
                FillInputs(*Driverquery);

                // Llegados hasta aca, tiene sentido que actualizamos el indice de registros
                RegIndex= IndexLocalList[Cedula];
            }
            else
            {
                EraseInputs();
            }
        }

        Connector->EndConnection();
    }
}

// Esta funcion se traera los datos del indice del apellido de entrada cuando se haga una busqueda
void DriversManager::UpdateInputsA(int Indice)
{
    // Simplemente llamemos al UpdateInputsC con la cedula correspondiente
    // a "Indice". Gracias al QMap "CedulaLocalList"
    if (Indice>0)
        UpdateInputsC(CedulaLocalList.value(Indice-1));
}

// Esta funcion se traera los datos de "Indice"
void DriversManager::UpdateInputsI(int Indice)
{
    // Simplemente llamemos al UpdateInputsC con la cedula correspondiente
    // a "Indice". Gracias al QMap "CedulaLocalList"
    if (Indice>=0)
        UpdateInputsC(CedulaLocalList.value(Indice));
}

// Esta funcion llenara los inputs con los datos contenidos en "input"
void DriversManager::FillInputs(QSqlQuery Input)
{
    ui->Nombre1Input->setText(Input.value(0).toString());
    ui->Nombre2Input->setText(Input.value(1).toString());
    ui->Apellido1Input->setText(Input.value(2).toString());
    ui->Apellido2Input->setText(Input.value(3).toString());
    ui->CedulaInput->setText(Input.value(5).toString());

    // Ahora para la fecha de nacimiento
    QString AuxFechaNacString= Input.value(4).toString();
    QStringList AuxFechaNac= AuxFechaNacString.split("-");
    ui->FechaNacInput->setDate(QDate(AuxFechaNac.at(0).toInt(), AuxFechaNac.at(1).toInt(), AuxFechaNac.at(2).toInt()));

    // Esta activo?
    ui->ActiveInput->setChecked(Input.value(6).toInt());

    // Y cambiamos el texto del boton de acuerdo al estado actual del Checkbox
    if (!ui->ActiveInput->isChecked())
        ui->AlternSuspendButton->setText("Habilitar");
    else
        ui->AlternSuspendButton->setText("Suspender");
}

// Esta funcion limpiara los inputs
void DriversManager::EraseInputs()
{
    ui->Nombre1Input->setText("");
    ui->Nombre2Input->setText("");
    ui->Apellido1Input->setText("");
    ui->Apellido2Input->setText("");
    ui->CedulaInput->setText("");
    ui->FechaNacInput->setDate(QDate::currentDate());
    ui->ActiveInput->setChecked(false);
}

// Esta funcion limpiara las estructuras de datos de la clase dejandola vacia
void DriversManager::EraseData()
{
    // Vaciamos los QMap
    IndexLocalList.clear();
    CedulaLocalList.clear();

    // Borramos la lista de apellidos, dejando el mensaje original
    ui->ApellidoSearchList->clear();
    ui->ApellidoSearchList->addItem("Buscar por apellido");

    // Inicializamos el indice de registros
    RegIndex= -1;

    // No estamos editando
    EDITING= false;
}

// Llena la interfaz con toda la informacion respectiva de la BD
void DriversManager::LoadData()
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
            // Si hemos llegado hasta aca, quiere decir que tenemos datos nuevos
            // con los que trabajar. Borremos primero las estructuras de datos
            EraseData();

            QStringList AuxLastNameList;
            QVector <QString> AuxCedulaList;
            DataQuery->first();

            // Vamos a llenar la lista de apellidos y la de cedulas, ambas auxiliares
            int k= 0;
            do
            {
                AuxLastNameList<<QString(DataQuery->value(0).toString()) + QString("-%1").arg(k);
                AuxCedulaList.append(DataQuery->value(1).toString());

                k++;
            }
            while (DataQuery->next());

            // Ahora la ordenamos
            AuxLastNameList.sort();

            // Y ahora vamos a proceder a llenar el mapa de cedulas y la lista de apellidos
            // para la navegacion
            for (int i=0; i<AuxLastNameList.count(); i++)
            {
                // Primero vamos a obtener el apellido con su respectivo indice
                QStringList AuxTokens= AuxLastNameList.at(i).split("-");

                // Ahora llenemos los mapas en el orden correcto
                CedulaLocalList.insert(i, AuxCedulaList.at(AuxTokens.at(1).toInt()));
                IndexLocalList.insert(AuxCedulaList.at(AuxTokens.at(1).toInt()), i);

                // Y llenemos la lista de apellidos visual
                ui->ApellidoSearchList->addItem(AuxTokens.at(0));
            }

            // Llenemos los campos
            UpdateInputsI(0);
            RegIndex= 0;
        }

        Connector->EndConnection();
    }
}

// Esta funcion evita que el usuario haga cosas como poner nombres con numeros o cedulas con letras
void DriversManager::ActivateValidators()
{
    // Creemos las validaciones de texto
    QRegExp TextReg("[a-zA-z]([a-zA-z]|[ ])*");
    QValidator *TextValidator= new QRegExpValidator(TextReg, this);

    ui->Nombre1Input->setValidator(TextValidator);
    ui->Nombre2Input->setValidator(TextValidator);
    ui->Apellido1Input->setValidator(TextValidator);
    ui->Apellido2Input->setValidator(TextValidator);

    // Creemos las validaciones numericas
    QRegExp NumReg("[1-9][0-9]*");
    QValidator *NumValidator= new QRegExpValidator(NumReg, this);

    ui->CedulaInput->setValidator(NumValidator);
    ui->CedulaSearchInput->setValidator(NumValidator);
}

// Esta funcion se encargara de leer los permisos del usuario actual y tomar decisiones en base a eso
bool DriversManager::UpdateUser (QString user)
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
            SetView("Inicio");

            // Listos para usar!
        }

        Connector->EndConnection();
        return true;
    }
}

// Este metodo actualizara la vista de acuerdo a variables como los permisos
// de usuario y/o la accion actual
void DriversManager::SetView(QString Modalidad)
{
    if (Modalidad=="default")
    {
        ui->TopFrame->hide();
        ui->ActionsFrame->hide();
    }
    if (Modalidad=="Inicio")
    {
        // Se muestran los bloques de la interfaz ahora que la clase ha
        // sido usada correctamente.
        ui->TopFrame->show();
        ui->ActionsFrame->show();

        // Bloqueo de Inputs
        ui->Nombre1Input->setReadOnly(true);
        ui->Nombre2Input->setReadOnly(true);
        ui->Apellido1Input->setReadOnly(true);
        ui->Apellido2Input->setReadOnly(true);
        ui->CedulaInput->setReadOnly(true);
        ui->FechaNacInput->setReadOnly(true);
        ui->ActiveInput->setEnabled(false);

        // Bloqueo de Acciones
        ui->SaveRegButton->hide();
        ui->CancelModButton->hide();
        if (PermissionRep->CanOnlyReadDriversManager())
            ui->ActionsFrame->hide();
        else
        {
            if (!PermissionRep->CanCreateDrivers())
                ui->NewButton->hide();
            if (!PermissionRep->CanEditDrivers())
                ui->ModButton->hide();
            if (!PermissionRep->CanSuspendDrivers())
                ui->AlternSuspendButton->hide();
            if (!PermissionRep->CanDeleteDrivers())
                ui->DelButton->hide();
        }

    }
    else
    if (Modalidad=="Nuevo")
    {
        //Limpieza de Inputs
        EraseInputs();

        // Desbloqueo de Inputs
        ui->Nombre1Input->setReadOnly(false);
        ui->Nombre2Input->setReadOnly(false);
        ui->Apellido1Input->setReadOnly(false);
        ui->Apellido2Input->setReadOnly(false);
        ui->CedulaInput->setReadOnly(false);
        ui->FechaNacInput->setReadOnly(false);
        ui->ActiveInput->setEnabled(true);

        // Bloqueo de Acciones
        ui->NewButton->hide();
        ui->DelButton->hide();
        ui->ModButton->hide();
        ui->AlternSuspendButton->hide();

        // Desbloqueo de Acciones
        ui->SaveRegButton->show();
        ui->CancelModButton->show();
    }
    else
    if (Modalidad=="Editar")
    {
        // Desbloqueo de Inputs
        ui->Nombre1Input->setReadOnly(false);
        ui->Nombre2Input->setReadOnly(false);
        ui->Apellido1Input->setReadOnly(false);
        ui->Apellido2Input->setReadOnly(false);
        ui->FechaNacInput->setReadOnly(false);
        ui->ActiveInput->setEnabled(true);

        // Bloqueo de Acciones
        ui->NewButton->hide();
        ui->ModButton->hide();
        ui->DelButton->hide();
        ui->AlternSuspendButton->hide();

        // Desbloqueo de Acciones
        ui->SaveRegButton->show();
        ui->CancelModButton->show();
    }
    else
    if (Modalidad=="Restaurar")
    {
        // Llenado de Inputs
        UpdateInputsI(RegIndex);

        // Bloqueo de Inputs
        ui->Nombre1Input->setReadOnly(true);
        ui->Nombre2Input->setReadOnly(true);
        ui->Apellido1Input->setReadOnly(true);
        ui->Apellido2Input->setReadOnly(true);
        ui->FechaNacInput->setReadOnly(true);
        ui->ActiveInput->setEnabled(false);

        // Desbloqueo de Acciones
        if (!PermissionRep->CanOnlyReadDriversManager())
        {
            if (PermissionRep->CanCreateDrivers())
                ui->NewButton->show();
            if (PermissionRep->CanEditDrivers())
                ui->ModButton->show();
            if (PermissionRep->CanDeleteDrivers())
                ui->DelButton->show();
            if (PermissionRep->CanSuspendDrivers())
                ui->AlternSuspendButton->show();
        }

        // Bloqueo de Acciones
        ui->SaveRegButton->hide();
        ui->CancelModButton->hide();
    }
    else
    if (Modalidad=="Primer Registro")
    {
        // Movimiento del Indice
        RegIndex=0;

        // Llenado de Inputs
        UpdateInputsI(RegIndex);

        // Bloquedo de Acciones
        ui->FirstRegButton->setEnabled(false);
        ui->PrevRegButton->setEnabled(false);

        // Desbloqueo de Acciones
        if (CedulaLocalList.count()>1)
        {
            ui->NextRegButton->setEnabled(true);
            ui->LastRegButton->setEnabled(true);
        }
    }
    else
    if (Modalidad=="Anterior Registro")
    {
        // Movimiento del Indice
        RegIndex--;

        if (RegIndex>0)
        {
            // Llenado de Inputs
            UpdateInputsI(RegIndex);

            // Desbloqueo de Acciones
            if (CedulaLocalList.count()>1)
            {
                ui->NextRegButton->setEnabled(true);
                ui->LastRegButton->setEnabled(true);
            }
        }
        else
            SetView("Primer Registro");
    }
    else
    if (Modalidad=="Siguiente Registro")
    {
        // Movimiento del Indice
        RegIndex++;

        if (RegIndex < CedulaLocalList.count()-1)
        {
            // Llenado de Inputs
            UpdateInputsI(RegIndex);

            // Desbloqueo de Acciones
            if (CedulaLocalList.count()>1)
            {
                ui->FirstRegButton->setEnabled(true);
                ui->PrevRegButton->setEnabled(true);
            }
        }
        else
            SetView("Ultimo Registro");
    }
    else
    if (Modalidad=="Ultimo Registro")
    {
        // Movimiento del Indice
        RegIndex= CedulaLocalList.count()-1;

        // Llenado de Inputs
        UpdateInputsI(RegIndex);

        // Bloquedo de Acciones
        ui->NextRegButton->setEnabled(false);
        ui->LastRegButton->setEnabled(false);

        // Desbloqueo de Acciones
        if (CedulaLocalList.count()>1)
        {
            ui->FirstRegButton->setEnabled(true);
            ui->PrevRegButton->setEnabled(true);
        }
    }
}

// Esto se ejecutara cuando se presione el boton "Nuevo"
void DriversManager::on_NewButton_clicked()
{
    SetView("Nuevo");
}

// Esto se ejecutara cuando se presione el boton "Modificar"
void DriversManager::on_ModButton_clicked()
{
    EDITING= true;
    SetView("Editar");
}

// Esto se ejecutara cuando se presione el boton "Cancelar"
void DriversManager::on_CancelModButton_clicked()
{
    EDITING= false;
    SetView("Restaurar");
}

// Esto se ejecutara cuando se presione el boton "Guardar"
void DriversManager::on_SaveRegButton_clicked()
{
    // Vamos a crear una conexion para poder traer ciertos datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
        "No se ha podido acceder a la Base de Datos."
        "Revise el estado de la misma.\n\nMensaje: Error DBA1\n"+
        Connector->getLastError().text());
    }
    else
    {
        // Los campos obligatorios estan llenos?
        QString ErrMess="";
        if (ui->Nombre1Input->text()=="")
            ErrMess+= QString("-Primer nombre<br>");
        if (ui->Apellido1Input->text()=="")
            ErrMess+= QString("-Primer apellido<br>");
        if (ui->Apellido2Input->text()=="")
            ErrMess+= QString("-Segundo apellido<br>");
        if (ui->CedulaInput->text()=="")
            ErrMess+= QString("-Cédula\n");

        if (ErrMess!="")
        {
            QMessageBox::information(0, "Error",
            "Los siguientes campos no pueden estar <b>vacios</b>:<br><br>"+
            ErrMess);
        }
        else
        {

            // Connector->Connector le dice al query con cual BD y conexion funcionar
            QSqlQuery* SaveQuery= new QSqlQuery (Connector->Connector);

            if (EDITING)
            {
                // Estamos editando un transportista
                if (!SaveQuery->exec(QString("UPDATE transportista SET ")+
                                     QString("primer_nombre='")+ui->Nombre1Input->text()+QString("', ")+
                                     QString("segundo_nombre='")+ui->Nombre2Input->text()+QString("', ")+
                                     QString("primer_apellido='")+ui->Apellido1Input->text()+QString("', ")+
                                     QString("segundo_apellido='")+ui->Apellido2Input->text()+QString("', ")+
                                     QString("fecha_nac='")+ui->FechaNacInput->date().toString("yyyy-MM-dd")+QString("', ")+
                                     QString("activo=%1 ").arg(ui->ActiveInput->isChecked())+
                                     QString("WHERE cedula=%1").arg(ui->CedulaInput->text().toInt())))
                    QMessageBox::critical(0, QObject::tr("Error"),
                    "No se han podido guardar los cambios. "
                    "Revise el estado de la Base de Datos.\n\nMensaje: Error DBQ1<br>");
                else
                {
                    // LoadData se encarga de poner EDITING en falso
                    LoadData();
                    SetView("Restaurar");
                }
            }
            else
            {
                // Estamos registrando un nuevo transportista

                // Existe?
                QSqlQuery* CheckQuery= new QSqlQuery (Connector->Connector);

                if (!CheckQuery->exec(QString("SELECT primer_nombre FROM transportista WHERE cedula=%1").arg(ui->CedulaInput->text().toInt())))
                    QMessageBox::critical(0, QObject::tr("Error"),
                    "No se ha podido verificar la existencia del transportista. "
                    "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBQ1");
                else
                {
                    if (CheckQuery->first())
                    {
                        QMessageBox::information(0, "Error",
                        "Ya existe un transportista con ese número de cédula.<br><br>"
                        "Si desea editar sus datos, intente buscar el numero de cédula indicado "
                        "en el cuadro \"Buscar por cédula\" y luego haga clic en \"Modificar\"");
                    }
                    else
                    {
                        // Llegados a este punto, ya podemos insertar sin problemas
                        QSqlQuery* InsertQuery= new QSqlQuery (Connector->Connector);

                        if (!InsertQuery->exec(QString("INSERT INTO transportista (primer_nombre, segundo_nombre, primer_apellido, "
                                                       "segundo_apellido, fecha_nac, cedula, activo) VALUES(")+
                                               QString("'")+ui->Nombre1Input->text()+QString("', ")+
                                               QString("'")+ui->Nombre2Input->text()+QString("', ")+
                                               QString("'")+ui->Apellido1Input->text()+QString("', ")+
                                               QString("'")+ui->Apellido2Input->text()+QString("', ")+
                                               QString("'")+ui->FechaNacInput->date().toString("yyyy-MM-dd")+QString("', ")+
                                               QString("%1").arg(ui->CedulaInput->text().toInt())+QString(", ")+
                                               QString("%1").arg(ui->ActiveInput->isChecked())+QString(")")))
                            QMessageBox::information(0, "Error",
                            "No se ha podido guardar el registro. Revise el estado de la base<br>"
                            "de datos.<br><br>"
                            "Mensaje: Error DBQ1<br>"+
                            Connector->getLastError().text());
                        else
                        {
                            LoadData();
                            SetView("Restaurar");
                        }
                    }
                }
            }
        }

        Connector->EndConnection();
    }
}

// Esto se ejecutara cuando se presione el boton "Eliminar"
void DriversManager::on_DelButton_clicked()
{
    QString Mensaje= "Esta a punto de eliminar al transportista <b>"+ui->Nombre1Input->text()+" "+ui->Apellido1Input->text()+"</b> de la base de datos.";
    QMessageBox* Confirmation= new QMessageBox(QMessageBox::Warning, "Confirmación requerida", Mensaje);
    Confirmation->setInformativeText("¿Está seguro?");

    QPushButton* Yes= new QPushButton("Si", this);
    QPushButton* Cancelar= new QPushButton("Cancelar", this);

    Confirmation->addButton(Yes, QMessageBox::YesRole);
    Confirmation->addButton(Cancelar, QMessageBox::RejectRole);
    Confirmation->setDefaultButton(Cancelar);

    int Answer = Confirmation->exec();

    if (Answer==0) //Si
    {
        // Vamos a crear una conexion para poder traer ciertos datos
        if (!Connector->RequestConnection())
        {
            QMessageBox::critical(0, "Error",
            "No se ha podido acceder a la Base de Datos."
            "Revise el estado de la misma.\n\nMensaje: Error DBA1\n"+
            Connector->getLastError().text());
        }
        else
        {
            // Connector->Connector le dice al query con cual BD y conexion funcionar
            QSqlQuery* DelQuery= new QSqlQuery (Connector->Connector);

            // Ejecutemos la eliminacion
            if (!DelQuery->exec(QString("DELETE FROM transportista WHERE cedula=%1").arg(ui->CedulaInput->text().toInt())))
            {
                QMessageBox::critical(0, "Error",
                "No se ha podido eliminar el registro."
                "<br><br>Mensaje: Error DBA1<br>"+
                Connector->getLastError().text());
            }
            else
            {
                // Exito en el borrado, actualizamos las estructuras y seguimos
                LoadData();
                SetView("Inicio");
            }

            Connector->EndConnection();
        }
    }
}

// Esto se ejecutara cuando se presione el boton "Suspender"
void DriversManager::on_AlternSuspendButton_clicked()
{
    // Para este caso vamos a desmarcar/marcar el checkbox de "Activo" y luego procedemos a guardar
    // todo
    ui->ActiveInput->setChecked(!ui->ActiveInput->isChecked());

    // Vamos a crear una conexion para poder traer ciertos datos
    if (!Connector->RequestConnection())
    {
        QMessageBox::critical(0, "Error",
        "No se ha podido acceder a la Base de Datos."
        "Revise el estado de la misma.\n\nMensaje: Error DBA1\n"+
        Connector->getLastError().text());
    }
    else
    {
        // Connector->Connector le dice al query con cual BD y conexion funcionar
        QSqlQuery* UpdQuery= new QSqlQuery (Connector->Connector);

        // Ejecutemos la eliminacion
        if (!UpdQuery->exec(QString("UPDATE transportista SET ")+
                            QString("activo=%1 ").arg(ui->ActiveInput->isChecked())+
                            QString("WHERE cedula=%1").arg(ui->CedulaInput->text().toInt())))
        {
            QMessageBox::critical(0, "Error",
            "No se ha podido actualizar el registro."
            "<br><br>Mensaje: Error DBA1<br>"+
            Connector->getLastError().text());
        }
        else
        {
            // Exito en la modificacion, actualizamos las estructuras y seguimos
            SetView("Restaurar");
        }

        Connector->EndConnection();
    }

    // Y cambiamos el texto del boton de acuerdo al estado actual del Checkbox
    if (!ui->ActiveInput->isChecked())
        ui->AlternSuspendButton->setText("Habilitar");
    else
        ui->AlternSuspendButton->setText("Suspender");
}

// Esto se ejecutara cuando se presione el boton "<|"
void DriversManager::on_FirstRegButton_clicked()
{
    SetView("Primer Registro");
}

// Esto se ejecutara cuando se presione el boton "<"
void DriversManager::on_PrevRegButton_clicked()
{
    SetView("Anterior Registro");
}

// Esto se ejecutara cuando se presione el boton ">"
void DriversManager::on_NextRegButton_clicked()
{
    SetView("Siguiente Registro");
}

// Esto se ejecutara cuando se presione el boton "|>"
void DriversManager::on_LastRegButton_clicked()
{
    SetView("Ultimo Registro");
}
