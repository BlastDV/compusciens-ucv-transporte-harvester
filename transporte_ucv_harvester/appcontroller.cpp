#include "appcontroller.h"
#include "stdio.h"

AppController::AppController(QObject *parent) : QObject(parent)
{
    Logger= new LogMaster(this);
}

//Esta funcion se encarga de arrancar con la aplicacion
void AppController::start()
{
    SessionW= new SessionWindow();

    connect(SessionW, SIGNAL(SesionAbierta()), this, SLOT(SesionAbierta()));
    //SessionW->show();

    LogRep= new LogReporter ();
    LogRep->UpdateUser("admin");
    LogRep->LoadEvents();
    //LogRep->UpdateUser(UserID);


    LogRep->show();
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

    //MainW->show();
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
