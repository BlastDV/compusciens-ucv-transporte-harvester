#include "permissionreporter.h"

PermissionReporter::PermissionReporter(QObject *parent) : QObject(parent)
{
    // Por default, usuario X no puede hacer nada
    // Permisos del Administrador de Usuarios
    READUSERSMANAGER= false;
    CREATEUSERS= false;
    EDITUSERS= false;
    DELETEUSERS= false;
    READONLYUSERSMANAGER= false;

    // Permisos del Log
    READALLLOGREP= false;
    WRITELOGREP= false;
    READONLYLOG= false;

    // Permisos del Administrador de Transportistas
    READDRIVERSMANAGER= false;
    CREATEDRIVERS= false;
    EDITDRIVERS= false;
    SUSPENDDRIVERS= false;
    DELETEDRIVERS= false;
    READONLYDRIVERSMANAGER= false;

}


// Esta es la funcion principal donde los permisos son decodificados
void PermissionReporter::CalculatePermissions(QString input)
{
    // Por default, usuario X no puede hacer nada
    // Permisos del Administrador de Usuarios
    READUSERSMANAGER= false;
    CREATEUSERS= false;
    EDITUSERS= false;
    DELETEUSERS= false;
    READONLYUSERSMANAGER= false;

    // Permisos del Log
    READALLLOGREP= false;
    WRITELOGREP= false;
    READONLYLOG= false;

    // Permisos del Administrador de Transportistas
    READDRIVERSMANAGER= false;
    CREATEDRIVERS= false;
    EDITDRIVERS= false;
    SUSPENDDRIVERS= false;
    DELETEDRIVERS= false;
    READONLYDRIVERSMANAGER= false;

    // Hora de Calcular!
    QStringList Permisos= input.split("/");

    // Siquiera tenemos algo que hacer?
    if (Permisos.count()>0)
    {
        //Aca empezamos a determinar lo que usuario X puede hacer
        /** Permisos del Administrador de Usuarios **/
        QStringList UsersMP= Permisos.at(0).split(".");
        // Puede ver el administrador?
        if (UsersMP.contains("R"))
            READUSERSMANAGER= true;
        // Puede agregar nuevos usuarios?
        if (UsersMP.contains("C"))
            CREATEUSERS= true;
        // Puede modificar los usuarios?
        if (UsersMP.contains("M"))
            EDITUSERS= true;
        // Puede eliminar usuarios?
        if (UsersMP.contains("D"))
            DELETEUSERS= true;
        // Solo puede ver el administrador?
        if (!CREATEUSERS & !EDITUSERS & !DELETEUSERS)
            READONLYUSERSMANAGER= true;

        /** Permisos del Log **/
        QStringList LogP=  Permisos.at(1).split(".");
        // Puede leer todo el log de eventos?
        if (LogP.contains("R"))
            READALLLOGREP= true;
        // Puede escribir el log de eventos?
        if (LogP.contains("W"))
            WRITELOGREP= true;
        // Solo puede leer?
        if (!READALLLOGREP & !WRITELOGREP)
            READONLYLOG= true;

        /** Permisos del Administrador de Transportistas **/
        QStringList DriversMP= Permisos.at(2).split(".");
        // Puede ver el administrador?
        if (DriversMP.contains("R"))
            READDRIVERSMANAGER= true;
        // Puede agregar nuevos transportistas?
        if (DriversMP.contains("C"))
            CREATEDRIVERS= true;
        // Puede modificar los transportistas?
        if (DriversMP.contains("M"))
            EDITDRIVERS= true;
        // Puede suspender transportistas?
        if (DriversMP.contains("S"))
            SUSPENDDRIVERS= true;
        // Puede eliminar transportistas?
        if (DriversMP.contains("D"))
            DELETEDRIVERS= true;
        // Solo puede ver el administrador?
        if (!CREATEDRIVERS & !EDITDRIVERS & !SUSPENDDRIVERS & !DELETEDRIVERS)
            READONLYDRIVERSMANAGER= true;
    }
}

// Usuario X puede ver el administrador de usuarios?
bool PermissionReporter::CanReadUsersManager()
{
    return READUSERSMANAGER;
}

// Usuario X puede crear usuarios?
bool PermissionReporter::CanCreateUsers()
{
    return CREATEUSERS;
}

// Usuario X puede modificar usuarios?
bool PermissionReporter::CanEditUsers()
{
    return EDITUSERS;
}

// Usuario X puede eliminar usuarios?
bool PermissionReporter::CanDeleteUsers()
{
    return DELETEUSERS;
}

// Usuario X solo puede ver el administrador de usuarios?
bool PermissionReporter::CanOnlyReadUsers()
{
    return READONLYUSERSMANAGER;
}

// Usuario X puede leer todo el log?
bool PermissionReporter::CanReadAllLog()
{
    return READALLLOGREP;
}

// Usuario X puede escribir el log?
bool PermissionReporter::CanWriteLog()
{
    return WRITELOGREP;
}

// Usuario X solo puede ver el log?
bool PermissionReporter::CanOnlyReadLog()
{
    return READONLYLOG;
}

// Usuario X puede ver el Administrador de Transportistas?
bool PermissionReporter::CanReadDriversManager()
{
    return READDRIVERSMANAGER;
}

// Usuario X puede crear nuevos transportistas?
bool PermissionReporter::CanCreateDrivers()
{
    return CREATEDRIVERS;
}

// Usuario X puede modificar los transportistas?
bool PermissionReporter::CanEditDrivers()
{
    return EDITDRIVERS;
}

// Usuario X puede suspender transportistas?
bool PermissionReporter::CanSuspendDrivers()
{
    return SUSPENDDRIVERS;
}

// Usuario X puede eliminar transportistas?
bool PermissionReporter::CanDeleteDrivers()
{
    return DELETEDRIVERS;
}

// Usuario X solo puede ver el administrador?
bool PermissionReporter::CanOnlyReadDriversManager()
{
    return READONLYDRIVERSMANAGER;
}

/* Esta clase tiene como unico fin el hacer debug sobre la misma*/
void PermissionReporter::DumpPermissions()
{
    qDebug("Permisos del Administrador de Usuarios:");
    qDebug("\tLeer: %d", READUSERSMANAGER);
    qDebug("\tCrear: %d", CREATEUSERS);
    qDebug("\tModificar: %d", EDITUSERS);
    qDebug("\tEliminar: %d", DELETEUSERS);
    qDebug("\tSolo Lectura: %d", READONLYUSERSMANAGER);

    qDebug("Permisos del Log:");
    qDebug("\tLeer Todo: %d", READALLLOGREP);
    qDebug("\tModificar: %d", WRITELOGREP);
    qDebug("\tSolo Lectura: %d", READONLYLOG);

    qDebug("Permisos del Administrador de Transportistas:");
    qDebug("\tLeer: %d", READDRIVERSMANAGER);
    qDebug("\tCrear: %d", CREATEDRIVERS);
    qDebug("\tModificar: %d", EDITDRIVERS);
    qDebug("\tSuspender: %d", SUSPENDDRIVERS);
    qDebug("\tEliminar: %d", DELETEDRIVERS);
    qDebug("\tSolo Lectura: %d", READONLYDRIVERSMANAGER);
}
