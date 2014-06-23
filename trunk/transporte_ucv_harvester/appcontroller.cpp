#include "appcontroller.h"
#include "stdio.h"

AppController::AppController(QObject *parent) : QObject(parent)
{
}

void AppController::start()
{
    reader= new Csp32Bridge();

    //printf("Ola ke ase: %d\n", reader->cspGetc());

    SessionW= new SessionWindow();
    SessionW->show();
}
