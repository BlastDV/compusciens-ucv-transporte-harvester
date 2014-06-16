#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "logreporter.h"

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

    QString UserID;

private:
    Ui::MainWindow *ui;
    LogReporter* LogRep;

private slots:
    void on_actionCerrarSesion_triggered();
    void on_actionVerLog_triggered();

signals:
    void CerrarSesion ();
    void ReportarAccion (QString action);
    void ShowLog ();
};

#endif // MAINWINDOW_H
