#include "usersmanager.h"
#include "ui_usersmanager.h"

UsersManager::UsersManager(QWidget *parent) : QMainWindow(parent), ui(new Ui::UsersManager)
{
    ui->setupUi(this);

    // Ningun usuario nos ve por ahora
    UserID= "default";

    // Este sera nuestro informante de permisos
    PermissionRep= new PermissionReporter(this);

    // No hay ningun registro seleccionado
    RegIndex= -1;

    // No estamos editando
    EDITING= false;
    EDITINGPASS= false;

    // Iniciamos el conector de la BD
    ConnectionName= "UsersManager";
    Connector= new DBConnector(this);
    Connector->ConnectionName= ConnectionName;

    // Este sera nuestro link con el log
    Logger= new LogMaster(this);
    Logger->SetConnectionName(ConnectionName + "-Logger");

    // Unimos las conexiones entre widgets
    connect(ui->UsersList, SIGNAL(cellClicked(int,int)), this, SLOT(UpdateView(int,int)));
    connect(ui->ShowPassInput, SIGNAL(toggled(bool)), this, SLOT(AlternPasswordShow(bool)));

    // Activemos las validaciones
    ActivateValidators();

    // Llenamos la interfaz
    LoadData();

    // Y la mostramos de forma preliminar, esto es: vacia. Por razones de seguridad
    // en posibles usos incorrectos de esta clase. Debe llamarse UpdateUser() antes de mostrarla!!!
    SetView("default");
}

UsersManager::~UsersManager()
{
    delete ui;
}

// Esta funcion se encargara de leer los permisos del usuario actual y tomar decisiones en base a eso
bool UsersManager::UpdateUser (QString user)
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

            // Cambiamos el User del Logger
            Logger->UpdateUser(UserID);

            // Listos para usar!
        }

        Connector->EndConnection();
        return true;
    }
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

                // Y ponemos RegIndex a apuntar al primer registro
                RegIndex= 0;
            }
        }

        Connector->EndConnection();
    }
}

// Este metodo limpia las estructuras locales
void UsersManager::EraseData()
{
    // Limpiemos la tabla
    while(ui->UsersList->rowCount()>0)
        ui->UsersList->removeRow(0);

    // Limpiemos el QMap de indices e IDs
    UsersLocalList.clear();
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

// Este metodo se encargara de esconder los inputs relacionados a la contraseña
void UsersManager::HidePassInputs()
{
    // Escondemos el apartado de "nueva contraseña"
    ui->NewPassLabel->hide();
    ui->NewPassInput->hide();
    ui->RepeatPassLabel->hide();
    ui->RepeatPassInput->hide();
    ui->ShowPassInput->hide();
    ui->CancelPassChangeButton->hide();

    // Mostramos las opciones de la contraseña actual
    ui->PasswordLabel->show();
    ui->ChangePassButton->show();
}

// Este metodo se encargara de mostrar los inputs relacionados a la contraseña
void UsersManager::ShowPassInputs()
{
    // Mostramos el apartado de "nueva contraseña"
    ui->NewPassLabel->show();
    ui->NewPassInput->show();
    ui->RepeatPassLabel->show();
    ui->RepeatPassInput->show();
    ui->ShowPassInput->show();
    if (EDITING)
        ui->CancelPassChangeButton->show();

    // Escondemos las opciones de la contraseña actual
    ui->PasswordLabel->hide();
    ui->ChangePassButton->hide();
}

// Este metodo alternata entre mostrar o no la contraseña
void UsersManager::AlternPasswordShow(bool checked)
{
    if (!checked)
    {
        ui->NewPassInput->setEchoMode(QLineEdit::Password);
        ui->RepeatPassInput->setEchoMode(QLineEdit::Password);
    }
    else
    {
        ui->NewPassInput->setEchoMode(QLineEdit::Normal);
        ui->RepeatPassInput->setEchoMode(QLineEdit::Normal);
    }
}

// Esta funcion evita que el usuario ingrese simbolos o espacios en el campo de id de usuario
// o espacios en el campo de contraseña
void UsersManager::ActivateValidators()
{
    // Creemos las validaciones del id de usuario
    QRegExp IDReg("([a-zA-Z]|[0-9])*");
    QValidator *IDValidator= new QRegExpValidator(IDReg, this);

    ui->IDInput->setValidator(IDValidator);

    // Creemos las validaciones del campo de contraseña
    QRegExp PassReg("([0-9]|[a-zA-Z]|[!#@\"_*-+]){4,25}");
    QValidator *PassValidator= new QRegExpValidator(PassReg, this);

    ui->NewPassInput->setValidator(PassValidator);
    ui->RepeatPassInput->setValidator(PassValidator);
}

// Esta funcion tomara los permisos marcados y los convertira en un QString
// como el que almancena la BD para los permisos
QString UsersManager::GetPermissions()
{
    QString Output="";
    int PermissionCounter;

    /** Revisemos los permisos del log **/
    PermissionCounter= 0;
    if (ui->CanReadAllLogInput->isChecked())
    {
        Output+="R.";
        PermissionCounter++;
    }
    if (ui->CanDeleteLogInput->isChecked())
    {
        Output+="W.";
        PermissionCounter++;
    }

    if (PermissionCounter==0)
        Output+="-/";
    else
        Output+="/";

    /** Revisemos los permisos del Administrador de Usuarios **/
    PermissionCounter= 0;
    if (ui->CanReadUsersInput->isChecked())
    {
        Output+="R.";
        PermissionCounter++;
    }
    if (ui->CanCreateUsersInput->isChecked())
    {
        Output+="C.";
        PermissionCounter++;
    }
    if (ui->CanEditUsersInput->isChecked())
    {
        Output+="M.";
        PermissionCounter++;
    }
    if (ui->CanDeleteUsersInput->isChecked())
    {
        Output+="D.";
        PermissionCounter++;
    }

    if (PermissionCounter==0)
        Output+="-/";
    else
        Output+="/";

    /** Revisemos los permisos del Administrador de Transportistas **/
    PermissionCounter= 0;
    if (ui->CanReadDriversInput->isChecked())
    {
        Output+="R.";
        PermissionCounter++;
    }
    if (ui->CanCreateDriversInput->isChecked())
    {
        Output+="C.";
        PermissionCounter++;
    }
    if (ui->CanEditDriversInput->isChecked())
    {
        Output+="M.";
        PermissionCounter++;
    }
    if (ui->CanSuspendDriversInput->isChecked())
    {
        Output+="S.";
        PermissionCounter++;
    }
    if (ui->CanDeleteDriversInput->isChecked())
    {
        Output+="D.";
        PermissionCounter++;
    }

    if (PermissionCounter==0)
        Output+="-/";
    else
        Output+="/";

    // Finalmente, retornemos el QString de permisos
    return Output;

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
        ui->ChangePassButton->setEnabled(false);
        HidePassInputs();
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
        ui->CancelModButton->hide();
        if (PermissionRep->CanOnlyReadUsers())
            ui->ActionsFrame->hide();
        else
        {
            if (!PermissionRep->CanCreateUsers())
                ui->NewButton->hide();
            if (!PermissionRep->CanEditUsers())
                ui->ModButton->hide();
            if (!PermissionRep->CanDeleteUsers())
                ui->DelButton->hide();
        }
    }
    else
    if (Modalidad=="Nuevo")
    {
        // Limpieza de Inputs
        EraseInputs();

        // Desbloqueo de Inputs
        ui->IDInput->setReadOnly(false);
        ShowPassInputs();
        ui->CanReadUsersInput->setEnabled(true);
        ui->CanCreateUsersInput->setEnabled(true);
        ui->CanEditUsersInput->setEnabled(true);
        ui->CanDeleteUsersInput->setEnabled(true);
        ui->CanReadAllLogInput->setEnabled(true);
        ui->CanDeleteLogInput->setEnabled(true);
        ui->CanReadDriversInput->setEnabled(true);
        ui->CanCreateDriversInput->setEnabled(true);
        ui->CanEditDriversInput->setEnabled(true);
        ui->CanSuspendDriversInput->setEnabled(true);
        ui->CanDeleteDriversInput->setEnabled(true);

        // Bloqueo de Acciones
        ui->UsersList->setEnabled(false);
        ui->NewButton->hide();
        ui->DelButton->hide();
        ui->ModButton->hide();

        // Desbloqueo de Acciones
        ui->SaveRegButton->show();
        ui->CancelModButton->show();
    }
    else
    if (Modalidad=="Editar")
    {
        // Desbloqueo de Inputs
        ui->ChangePassButton->setEnabled(true);
        ui->CanReadUsersInput->setEnabled(true);
        ui->CanCreateUsersInput->setEnabled(true);
        ui->CanEditUsersInput->setEnabled(true);
        ui->CanDeleteUsersInput->setEnabled(true);
        ui->CanReadAllLogInput->setEnabled(true);
        ui->CanDeleteLogInput->setEnabled(true);
        ui->CanReadDriversInput->setEnabled(true);
        ui->CanCreateDriversInput->setEnabled(true);
        ui->CanEditDriversInput->setEnabled(true);
        ui->CanSuspendDriversInput->setEnabled(true);
        ui->CanDeleteDriversInput->setEnabled(true);

        // Bloqueo de Acciones
        ui->UsersList->setEnabled(false);
        ui->NewButton->hide();
        ui->DelButton->hide();
        ui->ModButton->hide();

        // Desbloqueo de Acciones
        ui->SaveRegButton->show();
        ui->CancelModButton->show();
    }
    if (Modalidad=="Restaurar")
    {
        // Llenado de Inputs
        UpdateView(RegIndex, 3);

        // Borrado de los Inputs de contraseña
        ui->NewPassInput->setText("");
        ui->RepeatPassInput->setText("");

        // Bloqueo de Inputs
        ui->IDInput->setReadOnly(true);
        ui->ChangePassButton->setEnabled(false);
        HidePassInputs();
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

        // Desbloqueo de Acciones
        ui->UsersList->setEnabled(true);
        if (!PermissionRep->CanOnlyReadUsers())
        {
            if (PermissionRep->CanCreateUsers())
                ui->NewButton->show();
            if (PermissionRep->CanEditUsers())
                ui->ModButton->show();
            if (PermissionRep->CanDeleteUsers())
                ui->DelButton->show();
        }

        // Bloqueo de Acciones
        ui->SaveRegButton->hide();
        ui->CancelModButton->hide();
    }
}

// Esta funcion permitira que al hacer click en un usuario de la lista, se muestre
// su informacion en la interfaz
void UsersManager::UpdateView(int UserIndex, int Trash)
{
    // Ignoramos "Trash", es solo algo necesario por la arquitectura de Qt

    // En UserIndex tenemos el indice del usuario en el QMap que el <usuario> quiere consultar
    // Por eso actualizamos RegIndex
    RegIndex= UserIndex;

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

// Esto se ejecutara cuando se presione el boton "Nuevo"
void UsersManager::on_NewButton_clicked()
{
    SetView("Nuevo");
}

// Esto se ejecutara cuando se presione el boton "Modificar"
void UsersManager::on_ModButton_clicked()
{
    EDITING= true;
    SetView("Editar");
}

// Esto se ejecutara cuando se presione el boton "Guardar"
void UsersManager::on_SaveRegButton_clicked()
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
        QString ErrMess="";
        // Los campos obligatorios estan llenos?
        if (ui->IDInput->text()=="")
            ErrMess+= QString("-ID de usuario<br>");
        if (EDITINGPASS || !EDITING)
        {
            if (ui->NewPassInput->text()=="")
                ErrMess+= QString("-Nueva contraseña<br>");
            if (ui->RepeatPassInput->text()=="")
                ErrMess+= QString("-Repetir contraseña<br>");
        }

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
                // Esto evitara que se guarde la contaseña con errores de longitud
                bool PASSWORDOK= true;

                // Estamos editando un usuario
                QString Query= "UPDATE usuario SET ";

                if (EDITINGPASS)
                {
                    // Si la contraseña se modifico, hay que comprobar que los campos
                    // "Nueva contraseña" y "Repetir contraseña" sean iguales
                    if (ui->NewPassInput->text()!=ui->RepeatPassInput->text())
                    {
                        QMessageBox::information(0, "Error",
                        "Las contraseñas no coinciden, revise los campos <b>Nueva contraseña</b> y "
                        "<b>Repetir contraseña</b>.");
                    }
                    else
                    {
                        // Los campos tienen la longitud adecuada?
                        QString LenErr="";

                        if (ui->NewPassInput->text().length()<4)
                            LenErr+="-La contraseña debe tener mínimo 4 caracteres y un maximo de 25<br>";

                        if (LenErr!="")
                        {
                            PASSWORDOK= false;

                            QMessageBox::information(0, "Error",
                            "Se encontraron los siguientes errores:<br>"+
                            LenErr);
                        }
                        else
                        {
                            // Si son iguales, hay que incluir la contraseña cifrada en el UPDATE
                            // Hay que encriptar la clave con SHA1 para compararla con la de la BD
                            QCryptographicHash* Encrypter;
                            QString Encryptedpassword= (Encrypter->hash(QByteArray(ui->NewPassInput->text().toStdString().c_str()), QCryptographicHash::Sha1)).toHex();

                            Query+=QString("password='")+Encryptedpassword+QString("', ");
                            EDITINGPASS= false;
                        }
                    }
                }

                if (PASSWORDOK)
                {
                    // Actualicemos los permisos
                    Query+=QString("permisos='")+GetPermissions()+QString("' ");
                    Query+=QString("WHERE id='")+ui->IDInput->text()+QString("'");

                    if (!SaveQuery->exec(Query))
                        QMessageBox::critical(0, QObject::tr("Error"),
                                              "No se han podido guardar los cambios. "
                                              "Revise el estado de la Base de Datos.<br><br>nMensaje: Error DBQ1<br>");
                    else
                    {
                        // Reportamos
                        Logger->RegistrarEvento("MODIFICO AL USUARIO "+ui->IDInput->text());

                        EDITING= false;
                        SetView("Restaurar");
                    }
                }
            }
            else
            {
                // Estamos registrando un nuevo usuario

                // Existe?
                QSqlQuery* CheckQuery= new QSqlQuery (Connector->Connector);

                if (!CheckQuery->exec(QString("SELECT permisos FROM usuario WHERE id='")+
                                      ui->IDInput->text()+QString("'")))
                    QMessageBox::critical(0, QObject::tr("Error"),
                    "No se ha podido verificar la existencia del usuario. "
                    "Revise el estado de la Base de Datos.<br><br>Mensaje: Error DBQ1");
                else
                {
                    if (CheckQuery->first())
                    {
                        QMessageBox::information(0, "Error",
                        "Ya existe un usuario con ese ID y no puede ser duplicado.<br><br>"
                        "Si desea editar sus datos, intente buscarlo en la lista de usuarios, "
                        "haga clic en \"<b>"+
                        ui->IDInput->text()+
                        "</b>\" y luego haga clic en \"Modificar\"");
                    }
                    else
                    {
                        // Ahora veamos si las contraseñas son iguales
                        if (ui->NewPassInput->text()!=ui->RepeatPassInput->text())
                        {
                            QMessageBox::information(0, "Error",
                            "Las contraseñas no coinciden, revise los campos <b>Nueva contraseña</b> y "
                            "<b>Repetir contraseña</b>.");
                        }
                        else
                        {
                            // Veamos si las contraseñas tienen la longitud adecuada
                            if (ui->NewPassInput->text().length()<4)
                            {
                                    QMessageBox::information(0, "Error",
                                    "Se encontraron los siguientes errores:<br><br>"
                                    "-La contraseña debe tener mínimo 4 caracteres y un maximo de 25<br>");
                            }
                            else
                            {
                                // Todas las condiciones han sido cumplidas, toca insertar
                                // Hay que encriptar la clave con SHA1 para compararla con la de la BD
                                QCryptographicHash* Encrypter;
                                QString Encryptedpassword= (Encrypter->hash(QByteArray(ui->NewPassInput->text().toStdString().c_str()), QCryptographicHash::Sha1)).toHex();

                                QSqlQuery* InsertQuery= new QSqlQuery (Connector->Connector);

                                if (!InsertQuery->exec(QString("INSERT INTO usuario (id, password, permisos) VALUES(")+
                                                       QString("'")+ui->IDInput->text()+QString("', ")+
                                                       QString("'")+Encryptedpassword+QString("', ")+
                                                       QString("'")+GetPermissions()+QString("')")))
                                    QMessageBox::information(0, "Error",
                                    "No se ha podido guardar el registro. Revise el estado de la base<br>"
                                    "de datos.<br><br>"
                                    "Mensaje: Error DBQ1<br>"+
                                    Connector->getLastError().text());
                                else
                                {
                                    // Reportamos
                                    Logger->RegistrarEvento("AGREGO AL USUARIO "+ui->IDInput->text());

                                    LoadData();
                                    SetView("Restaurar");
                                }
                            }
                        }

                    }
                }
            }
        }


        Connector->EndConnection();
    }
}

// Esto se ejecutara cuando se presione el boton "Cancelar"
void UsersManager::on_CancelModButton_clicked()
{
    EDITING= false;
    EDITINGPASS= false;
    ui->NewPassInput->setText("");
    ui->RepeatPassInput->setText("");
    SetView("Restaurar");
}

// Esto se ejecutara cuando se presione el boton "Eliminar"
void UsersManager::on_DelButton_clicked()
{
    // Primero solicitamos una confirmacion
    QString Mensaje= "Esta a punto de eliminar al usuario <b>"+ui->IDInput->text()+"</b> de la base de datos.";
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
            if (!DelQuery->exec(QString("DELETE FROM usuario WHERE id='")+
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
                Logger->RegistrarEvento("ELIMINO AL USUARIO "+ui->IDInput->text());

                // Exito en el borrado, actualizamos las estructuras y seguimos
                LoadData();
                SetView("Inicio");
            }

            Connector->EndConnection();
        }
    }
}

// Esto se ejecutara cuando se presione el boton "Cambiar" (contraseña)
void UsersManager::on_ChangePassButton_clicked()
{
    EDITINGPASS= true;
    ShowPassInputs();
}

// Esto se ejecutara cuando se presione el boton "Cancelar Cambio de Contraseña"
void UsersManager::on_CancelPassChangeButton_clicked()
{
    EDITINGPASS= false;
    ui->NewPassInput->setText("");
    ui->RepeatPassInput->setText("");
    HidePassInputs();
}
