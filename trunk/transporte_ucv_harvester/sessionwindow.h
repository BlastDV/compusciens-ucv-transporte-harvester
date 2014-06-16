#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QWidget>
#include <QCryptographicHash>
#include <QtSql/QtSql>

//Clases para el resto del programa
#include "mainwindow.h"
#include "logmaster.h"
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

private slots:
    void on_SessionSubmitButton_clicked();
    void CerrarSesion ();

private:
    Ui::SessionWindow *ui;
    QCryptographicHash *Encrypter;
    DBConnector* Connector;

    QString UserID;
    LogMaster* Logger;
    MainWindow* w;


    bool InicioSesion (QString UserID, QString Password);

signals:
    void ReportarUserID (QString UserID);
    void ReportarAccion (QString action);
};

#endif // SESSIONWINDOW_H
