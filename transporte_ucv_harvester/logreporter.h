/* Esta clase sera la encargada de permitir al usuario la visualizacion
 * del log de eventos del sistema. De acuerdo al tipo de usuario y sus credenciales,
 * este podra ver informacion de los demas usuarios o solo la propia. */

#ifndef LOGREPORTER_H
#define LOGREPORTER_H

#include <QWidget>
#include "dbconnector.h"

namespace Ui
{
    class LogReporter;
}

class LogReporter : public QWidget
{
    Q_OBJECT
    
public:
    explicit LogReporter(QWidget *parent = 0);
    ~LogReporter();

    QString ConnectionName;
    bool UpdateUser (QString user);
    bool LoadEvents();
    
private:
    Ui::LogReporter *ui;
    QString UserID;
    DBConnector* Connector;

protected:


};

#endif // LOGREPORTER_H
