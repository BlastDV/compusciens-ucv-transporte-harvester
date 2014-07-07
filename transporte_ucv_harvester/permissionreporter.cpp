#include "permissionreporter.h"

PermissionReporter::PermissionReporter(QObject *parent) : QObject(parent)
{
    // Por default, usuario X no puede hacer nada
    READALLLOGREP= false;
    WRITELOGREP= false;
}


// Esta es la funcion principal donde los permisos son decodificados
void PermissionReporter::CalculatePermissions(QString input)
{
    QStringList Permisos= input.split("/");

    // Si quiera tenemos algo que hacer?
    if (Permisos.count()>0)
    {
        //Aca empezamos a determinar lo que usuario X puede hacer

        // Puede leer y escribir todo el log de eventos?
        if (Permisos.at(0)=="RW")
        {
            READALLLOGREP= true;
            WRITELOGREP= true;
        }
        // Puede leer todo el log de eventos?
        else if (Permisos.at(0)=="R")
            READALLLOGREP= true;


    }
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

/* Esta clase tiene como unico fin el hacer debug sobre la misma*/
void PermissionReporter::DumpPermissions()
{

}
