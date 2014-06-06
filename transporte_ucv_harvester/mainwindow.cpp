#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName("localhost");
    db.setDatabaseName("test");
    db.setUserName("compusciens");
    db.setPassword("kakolukiya");
    db.setPort(3306);
    if (!db.open())
    {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                                  db.lastError().text());

    }
    else
    {
        printf("ola ke ase\n");
    }
        /*
    if (ok)
    {
        printf("ola ke ase\n");
    }
    else
        printf("noup\n");*/
}

MainWindow::~MainWindow()
{
    delete ui;
}
