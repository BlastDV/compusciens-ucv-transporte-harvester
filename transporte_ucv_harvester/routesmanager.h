#ifndef ROUTESMANAGER_H
#define ROUTESMANAGER_H

#include <QWidget>
#include <QMessageBox>
#include "dbconnector.h"
#include "permissionreporter.h"
#include "logmaster.h"

namespace Ui
{
    class RoutesManager;
}

class RoutesManager : public QWidget
{
    Q_OBJECT

public:
    explicit RoutesManager(QWidget *parent = 0);
    ~RoutesManager();
    bool UpdateUser (QString user);

private:
    Ui::RoutesManager *ui;
    DBConnector* Connector;
    LogMaster* Logger;
    PermissionReporter* PermissionRep;
    QString ConnectionName;
    QString UserID;


    // Estos metodos se encargan del apartado visual de los
    // campos y de la extraccion de datos de la BD
    void LoadData();
    void FillInputs(QSqlQuery Input);
};

#endif // ROUTESMANAGER_H
