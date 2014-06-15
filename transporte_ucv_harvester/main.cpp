#include <QApplication>
#include "sessionwindow.h"
  
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SessionWindow s;
    s.show();

    return a.exec();
}
