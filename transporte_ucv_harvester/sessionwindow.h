/* Esta clase maneja lo relacionado al inicio de sesion, encriptando la clave
 * ofrecida por el usuario para compararla con la que podria encontrarse en la BD,
 * y permitiendo el flujo de la aplicacion si las credenciales son correctas. */

#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QWidget>
#include <QCryptographicHash>
#include <QtSql/QtSql>

//Clases para el resto del programa
#include "dbconnector.h"

namespace Ui
{
    class SessionWindow;
}

class SessionWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SessionWindow(QWidget *parent = 0);
    ~SessionWindow();

    QString ConnectionName;

    QString getUserID();
    void ResetInputs();

private slots:
    void on_SessionSubmitButton_clicked();

private:
    Ui::SessionWindow *ui;
    QCryptographicHash *Encrypter;
    DBConnector* Connector;

    QString UserID;

    bool InicioSesion (QString User, QString Password);

signals:
    void ReportarUserID (QString UserID);
    void ReportarAccion (QString action);
    void SesionAbierta ();
};

#endif // SESSIONWINDOW_H
