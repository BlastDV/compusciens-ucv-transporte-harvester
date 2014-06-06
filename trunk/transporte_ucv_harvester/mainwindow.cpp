#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName("localhost");
    db.setDatabaseName("ptransporteucv");
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
        QSqlQuery query;
        query.exec("SELECT nombre FROM persona WHERE cedula=21536559");

        query.first();

        QMessageBox::critical(0, QObject::tr("Database Result"),
                                  query.value(0).toString());

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
