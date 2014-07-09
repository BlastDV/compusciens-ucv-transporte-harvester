#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "logreporter.h"
#include "deviceconnector.h"
#include "dbconnector.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString UserID;
    QString ConnectionName;

    bool UpdateUser(QString User);

private:
    Ui::MainWindow *ui;
    LogReporter* LogRep;
    DBConnector* Connector;
    DeviceConnector* DevConnector; // Para la ventana de conexion con el lector

    void LoadInitialData(); // Para la ventana de subida de datos inicial

private slots:
    void on_actionCerrarSesion_triggered();
    void on_actionVerLog_triggered();

    void on_actionConectarDispositivo_triggered();
    void DeviceConnectionAborted();
    void ReportarMensaje(QString mensaje);


signals:
    void CerrarSesion ();
    void ReportarAccion (QString action);
    void ShowLog ();
};

#endif // MAINWINDOW_H
