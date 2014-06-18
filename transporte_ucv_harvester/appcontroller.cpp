#include "appcontroller.h"

AppController::AppController(QObject *parent) : QObject(parent)
{
}

void AppController::start()
{
    SessionW= new SessionWindow();
    SessionW->show();
}
