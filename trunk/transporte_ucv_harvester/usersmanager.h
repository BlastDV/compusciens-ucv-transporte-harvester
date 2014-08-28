#ifndef USERSMANAGER_H
#define USERSMANAGER_H

#include <QMainWindow>
#include <QMessageBox>
#include "dbconnector.h"
#include "permissionreporter.h"

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


    
private:
    Ui::UsersManager *ui;
    DBConnector* Connector;
    PermissionReporter* PermissionRep;
    QString ConnectionName;
    QString UserID;
    bool EDITING;

    QMap <int, QString> UsersLocalList;

    // Estos metodos se encargan del apartado visual de los
    // campos y de la extraccion de datos de la BD
    void LoadData();
    void EraseData();
    void FillInputs(QSqlQuery Input);
    void EraseInputs();
    void SetView(QString Modalidad);

private slots:
    void UpdateView(int UserIndex, int Trash);
};

#endif // USERSMANAGER_H
