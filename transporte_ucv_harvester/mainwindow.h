#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "logreporter.h"
#include "deviceconnector.h"

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

    DeviceConnector* DevConnector; // Para la ventana de conexion con el lector

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
