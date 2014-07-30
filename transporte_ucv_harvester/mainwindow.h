#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtGui>
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

    // Esta lista tendra todos los codigos recuperados del dispositivo. Permitira que las operaciones
    // que dependan de los mismos sean mas rapidas
    QList <QString> CodesList;

    // Esta otra tendra las cedulas de todos los transportistas en la BD
    QList <QString> CedulaList;

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

    // Esta funcion es una de las primordiales
    void ReadCodes();
    // A su vez se apoyara en esta
    void CalculateTrips (QString cedula);
    void CalculateTrips ();

    // Esta recibe la actualizacion de DeviceConnector y actualiza
    // la barra de progreso de la interfaz
    void GetReadingUpdate(int);

    void on_CalculateTripsButton_clicked();

    void on_DriverReadyButton_clicked();

    void on_BackToDriverButton_clicked();

    void on_FindTransportistButton_clicked();

signals:
    void CerrarSesion ();
    void ReportarAccion (QString action);
    void ShowLog ();
};

#endif // MAINWINDOW_H
