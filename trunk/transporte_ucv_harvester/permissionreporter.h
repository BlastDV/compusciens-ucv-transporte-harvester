/* Esta clase sera la encargada de leer una serie de permisos
 * de la BD y organizarlos de forma que puedan ser accesibles
 * por otros modulos de la aplicacion. Basicamente tiene como entrada
 * un QString que define los permisos (Algo parecido a RWNRWWWR), esta
 * es decodificada y actualizada en los atributos de la clase.
 * Finalmente, provee metodos para reportar que cosas puede
 * o no puede hacer un determinado usuario. Por ejemplo, el modulo
 * "LogReporter" podria preguntar: "Usuario "kakolukiya" puede ver
 * el registro de eventos global?" y esta clase respondera segun
 * lo que sus permisos definan.
 */

#ifndef PERMISSIONREPORTER_H
#define PERMISSIONREPORTER_H

#include <QObject>
#include <QStringList>

class PermissionReporter : public QObject
{
    Q_OBJECT
public:
    explicit PermissionReporter(QObject *parent = 0);

    void CalculatePermissions(QString input);

    // Permisos del Log
    bool CanReadAllLog ();
    bool CanWriteLog ();
    bool CanOnlyReadLog();

    // Permisos del Administrador de Transportistas
    bool CanReadDriversManager();
    bool CanCreateDrivers();
    bool CanEditDrivers();
    bool CanSuspendDrivers();
    bool CanDeleteDrivers();
    bool CanOnlyReadDriversManager();

    void DumpPermissions();

signals:

private:
    // Permisos del Log
    bool READALLLOGREP;
    bool WRITELOGREP;
    bool READONLYLOG;

    // Permisos del Administrador de Transportistas
    bool READDRIVERSMANAGER;
    bool CREATEDRIVERS;
    bool EDITDRIVERS;
    bool SUSPENDDRIVERS;
    bool DELETEDRIVERS;
    bool READONLYDRIVERSMANAGER;


public slots:


};

#endif // PERMISSIONREPORTER_H
