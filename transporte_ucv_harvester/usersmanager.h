#ifndef USERSMANAGER_H
#define USERSMANAGER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QCryptographicHash>
#include "dbconnector.h"
#include "permissionreporter.h"
#include "logmaster.h"

namespace Ui
{
    class UsersManager;
}

class UsersManager : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit UsersManager(QWidget *parent = 0);
    ~UsersManager();
    bool UpdateUser (QString user);

    
private:
    Ui::UsersManager *ui;
    DBConnector* Connector;
    LogMaster* Logger;
    PermissionReporter* PermissionRep;
    QString ConnectionName;
    QString UserID;
    bool EDITING;
    bool EDITINGPASS;

    QMap <int, QString> UsersLocalList;
    int RegIndex;

    // Estos metodos se encargan del apartado visual de los
    // campos y de la extraccion de datos de la BD
    void LoadData();
    void EraseData();
    void FillInputs(QSqlQuery Input);
    void EraseInputs();
    void SetView(QString Modalidad);
    void HidePassInputs();
    void ShowPassInputs();

    // Este metodo colocara restricciones a todos los intpus
    void ActivateValidators();

    // Este metodo permitira procesar los permisos marcados en la interfaz
    QString GetPermissions();

private slots:
    void UpdateView(int UserIndex, int Trash);
    void AlternPasswordShow(bool checked);
    void on_NewButton_clicked();
    void on_ModButton_clicked();
    void on_SaveRegButton_clicked();
    void on_CancelModButton_clicked();
    void on_DelButton_clicked();
    void on_ChangePassButton_clicked();
    void on_CancelPassChangeButton_clicked();
};

#endif // USERSMANAGER_H
