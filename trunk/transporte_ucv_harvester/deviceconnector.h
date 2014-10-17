#ifndef DEVICECONNECTOR_H
#define DEVICECONNECTOR_H

#include <QWidget>

#include "csp32bridge.h"
#include "logmaster.h"

namespace Ui
{
    class DeviceConnector;
}

class DeviceConnector : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceConnector(QWidget *parent = 0);
    ~DeviceConnector();

    void InitObject(); // Esta clase debe llamarse cada vez que se muestre esto

    /* Aunque probablemente esta no sea la mejor solucion, poner este objeto
     * como publico facilita mucho las operaciones desde MainWindow. Por los
     * momentos nos iremos por esta via */
    Csp32Bridge* Reader;


private slots:
    void BloquearComandos();
    void DesbloquearComandos();
    void HabilitarComandos(QString scenario);
    void CheckCOMPorts();
    void RestoreObject(); // Clase para reiniciar el objeto
    void GetReadingUpdate(int progreso); // Recibe la señal de actualizacion de la clase Csp32Bridge

    void on_COM1Button_clicked();
    void on_COM2Button_clicked();
    void on_COM3Button_clicked();
    void on_COM4Button_clicked();
    void on_InitButton_clicked();
    void on_RestoreButton_clicked();
    void on_CancelButton_clicked();
    void on_AcceptButton_clicked();


    void on_ShowSettingsButton_clicked();

    void on_ReadSettingsButton_clicked();

private:
    Ui::DeviceConnector *ui;


    LogMaster* Logger;
    QString UserID;

    int SelectedCOM;

signals:
    /* Esto informa a la clase padre cuando
     * el usuario presione Cancelar*/
    void CancelPressed();
    void AcceptPressed(); // El usuario guardo los cambios

    // Esto informa a la clase padre sobre el progreso del proceso de recuperacion de codigos
    void ReadingCodes(int progreso);

    // Esto permite a appcontroller registrar las acciones del usuario
    void RegistrarEvento(QString);
};

#endif // DEVICECONNECTOR_H
