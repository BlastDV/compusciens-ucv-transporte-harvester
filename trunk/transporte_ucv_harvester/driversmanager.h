#ifndef DRIVERSMANAGER_H
#define DRIVERSMANAGER_H

#include <QWidget>
#include <QMessageBox>
#include "dbconnector.h"

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
    
private:
    Ui::DriversManager *ui;
    DBConnector* Connector;

    int RegIndex;
    QMap <QString, int> CedulaLocalList; // Esto permitira
    // hacer la busqueda por apellido correctamente y simplificara
    // la navegacion a traves de los registros

    // Este metodo llenara la interfaz con el primer registro
    // de transportistas de la BD
    void LoadInitialData();
    void FillInputs(QSqlQuery Input);
};

#endif // DRIVERSMANAGER_H
