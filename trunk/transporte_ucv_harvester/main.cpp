#include <QApplication>
#include "appcontroller.h"
#include "driversmanager.h"
  
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppController AppC;
    AppC.start();

    return a.exec();
}
