/* Esta clase tiene por finalidad controlar todo el flujo de la aplicacion,
 * cuando se abre una sesión o se cierra, por ejemplo, esta se encargara de crear
 * las instancias respectivas de las nuevas ventanas */

#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>

//Clases para el resto del programa
#include "mainwindow.h"
#include "sessionwindow.h"
#include "logmaster.h"
#include "dbconnector.h"

//Clases de prueba para el desarrollo, remover al construir la version final!!!
#include "logreporter.h"
#include "deviceconnector.h"

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = 0);

private:
    MainWindow* MainW;
    SessionWindow* SessionW;

    LogMaster* Logger;
    LogReporter* LogRep;
    DBConnector* Connector;

    DeviceConnector* DevConnector;

signals:

public slots:
    void SesionAbierta();
    void SesionCerrada();

public:
    void start();

};

#endif // APPCONTROLLER_H
