#include "mainwindow.h"
#include <QApplication>
#include "sessionwindow.h"
  
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    SessionWindow s;
    s.show();

    return a.exec();
}
