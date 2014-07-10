#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "logreporter.h"
#include "deviceconnector.h"
#include "dbconnector.h"
#include "csp32bridge.h"

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
    DeviceConnector* DevConnector; // Para la ventana de conexion con el lector y la comunicacion
                                    // con el mismo
    QMap <QString, int> RouteLocalList; // Esto nos permitira disminuir la cantidad de

    // consultas a la BD al momento de actualizar la lista de paradas dinamicamente

    void LoadInitialData(); // Para la ventana de subida de datos inicial

private slots:
    void on_actionCerrarSesion_triggered();
    void on_actionVerLog_triggered();

    void on_actionConectarDispositivo_triggered();
    void DeviceConnectionAborted();
    void DeviceConnectionAccepted();
    void ReportarMensaje(QString mensaje);

    void UpdateTransportistaC(QString cedula); // Por razones internas de Qt
    void UpdateTransportistaA(QString apellido);
    void UpdateRoute(QString id);

    // Esta funcion es la que mas nos interesa
    void ReadCodes();

    void on_ReadCodesButton_clicked();

signals:
    void CerrarSesion ();
    void ReportarAccion (QString action);
    void ShowLog ();
};

#endif // MAINWINDOW_H
