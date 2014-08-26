#include "appcontroller.h"
#include "stdio.h"

AppController::AppController(QObject *parent) : QObject(parent)
{
    Logger= new LogMaster(this);
}

//Esta funcion se encarga de arrancar con la aplicacion
void AppController::start()
{
    //MainW= new MainWindow();
    //MainW->UpdateUser(SessionW->getUserID());
    //MainW->show();

    SessionW= new SessionWindow();

    connect(SessionW, SIGNAL(SesionAbierta()), this, SLOT(SesionAbierta()));
    SessionW->show();

    //LogRep= new LogReporter ();
    //LogRep->UpdateUser("admin");
    //LogRep->LoadEvents();
    //LogRep->UpdateUser(UserID);

    //LogRep->show();
}

//Esto se activara al abrir sesion correctamente
void AppController::SesionAbierta()
{
    Logger->UpdateUser(SessionW->getUserID());
    SessionW->close();

    if (Logger->RegistrarEvento("INICIO SESION"))
        QMessageBox::information(0, QObject::tr("Gud"), "Evento Registrado");

    MainW= new MainWindow();
    MainW->UpdateUser(SessionW->getUserID());

    connect(MainW, SIGNAL(CerrarSesion()), this, SLOT(SesionCerrada()));
    connect(MainW, SIGNAL(ReportarAccion(QString)), Logger, SLOT(RegistrarEvento(QString)));

    MainW->show();
}

void AppController::SesionCerrada()
{
    disconnect (MainW, SIGNAL(CerrarSesion()));
    disconnect (MainW, SIGNAL(ReportarAccion(QString)));

    // Registramos el evento en la BD
    if (Logger->RegistrarEvento("CERRO SESION"))
        QMessageBox::information(0, QObject::tr("Gud"), "Evento Registrado");

    MainW->close();

    SessionW->ResetInputs();
    SessionW->show();
}

/* Esta funcion permitira registrar en el Log cualquier actividad llevada
 * a cabo por el usuario en cualquiera de los modulos. A traves de la
 * arquitectura de Señal->Slot, los mensajes de cualquier clase terminaran en
 * este punto */
void AppController::RegistrarAccion(QString accion)
{
    // Registramos el evento en la BD
    if (Logger->RegistrarEvento(accion))
        QMessageBox::information(0, QObject::tr("Gud"), "Evento Registrado");

    // [debug] EL MENSAJE ANTERIOR DEBE SER REMOVIDO ANTES DE COMPILAR LA VERSION RELEASE
}
