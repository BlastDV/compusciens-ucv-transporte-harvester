#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <stdio.h>
#include <QMessageBox>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSqlDatabase DBConnector;

private:
    Ui::MainWindow *ui;


private slots:
    void on_actionCerrarSesion_triggered();

signals:
    void CerrarSesion ();
    void ReportarAccion (QString action);
};

#endif // MAINWINDOW_H
