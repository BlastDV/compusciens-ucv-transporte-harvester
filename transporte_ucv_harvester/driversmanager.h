#ifndef DRIVERSMANAGER_H
#define DRIVERSMANAGER_H

#include <QWidget>
#include <QMessageBox>
#include "dbconnector.h"
#include "permissionreporter.h"

namespace Ui
{
    class DriversManager;
}

class DriversManager : public QWidget
{
    Q_OBJECT
    
public:
    explicit DriversManager(QWidget *parent = 0);
    ~DriversManager();

    QString ConnectionName;
    bool UpdateUser (QString user);
    
private:
    Ui::DriversManager *ui;
    DBConnector* Connector;
    PermissionReporter* PermissionRep;
    QString UserID;

    bool EDITING;
    int RegIndex;
    QMap <QString, int> IndexLocalList; // Esto
    QMap <int, QString> CedulaLocalList; // permitira
    // hacer la busqueda por apellido correctamente y simplificara
    // la navegacion a traves de los registros

    // Este metodo llenara la interfaz con el primer registro
    // de transportistas de la BD
    void LoadData();
    void EraseData();
    void FillInputs(QSqlQuery Input);
    void EraseInputs();

    // Este metodo se encargara de modificar la vista de acuerdo a la accion
    // actual
    void SetView(QString Modalidad);

    // Este metodo colocara restricciones a todos los intpus
    void ActivateValidators();

private slots:
    void UpdateInputsC(QString Cedula);
    void UpdateInputsA(int Indice);
    void UpdateInputsI(int Indice);

    void on_NewButton_clicked();
    void on_ModButton_clicked();
    void on_CancelModButton_clicked();
    void on_SaveRegButton_clicked();
    void on_DelButton_clicked();
    void on_AlternSuspendButton_clicked();
    void on_FirstRegButton_clicked();
    void on_PrevRegButton_clicked();
    void on_NextRegButton_clicked();
    void on_LastRegButton_clicked();
};

#endif // DRIVERSMANAGER_H
