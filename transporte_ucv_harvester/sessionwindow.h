#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QWidget>
#include <QCryptographicHash>
#include <QtSql/QtSql>

namespace Ui
{
    class SessionWindow;
}

class SessionWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SessionWindow(QWidget *parent = 0);
    ~SessionWindow();

private slots:
    void on_SessionSubmitButton_clicked();

private:
    Ui::SessionWindow *ui;
    QCryptographicHash *Encrypter;
    QSqlDatabase DBConnector;

    bool InicioSesion (QString UserID, QString Password);
};

#endif // SESSIONWINDOW_H
