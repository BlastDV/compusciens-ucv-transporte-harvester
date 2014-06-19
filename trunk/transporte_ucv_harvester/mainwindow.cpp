#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Cuando se haga click en "Cerrar Sesion", se emitira una señal a SessionWindow para que haga lo propio
void MainWindow::on_actionCerrarSesion_triggered()
{
    emit CerrarSesion();
}

// Cuando se haga click en "Ver el registro de Sucesos", se ejecutara lo siguiente que consiste en crear el objeto LogReporter
// y mostrarlo, para posteriormente destruirlo
void MainWindow::on_actionVerLog_triggered()
{
    LogRep= new LogReporter (this);

    QMessageBox::critical(0, QObject::tr("Gud"), "YEAH");
    LogRep->UserID= UserID;

    LogRep->show();
}
