/* Esta clase sera la encargada de permitir al usuario la visualizacion
 * del log de eventos del sistema. De acuerdo al tipo de usuario y sus credenciales,
 * este podra ver informacion de los demas usuarios o solo la propia. */

#ifndef LOGREPORTER_H
#define LOGREPORTER_H

#include <QWidget>
#include <QMessageBox>
#include "dbconnector.h"
#include "permissionreporter.h"

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
    PermissionReporter* PermissionRep;
    QVector <int> EventsLocalList;

private slots:
    void FilterUsers (QString userid);
    void MarkRow (int row, int column);

    void on_DeleteButton_clicked();

protected:


};

#endif // LOGREPORTER_H
