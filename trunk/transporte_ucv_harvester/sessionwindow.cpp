#include "sessionwindow.h"
#include "ui_sessionwindow.h"
#include "stdio.h"
#include <QMessageBox>

SessionWindow::SessionWindow(QWidget *parent) : QWidget(parent), ui(new Ui::SessionWindow)
{
    ui->setupUi(this);

}

SessionWindow::~SessionWindow()
{
    delete ui;
}

bool SessionWindow::InicioSesion(QString UserID, QString Password)
{
    bool INICIOOK; // Para devolver el resultado de la operacion

    // Vamos a crear una conexion para poder autentificar al usuario
    DBConnector= QSqlDatabase::addDatabase("QODBC");
    DBConnector.setHostName("localhost");
    DBConnector.setDatabaseName("ptransporteucv");
    DBConnector.setUserName("compusciens");
    DBConnector.setPassword("kakolukiya");
    DBConnector.setPort(3306);

    if (!DBConnector.open()) // Error en la BD. No disponible?
    {
        QMessageBox::critical(0, QObject::tr("Error"),
        "No se ha podido iniciar sesión, revise el estado"
        "de la Base de Datos\n\nMensaje: Error DBA1\n"+ DBConnector.lastError().text());

        INICIOOK= false;
    }
    else
    {
        // Hay que encriptar la clave con SHA1 para compararla con la de la BD
        QString Encryptedpassword= (Encrypter->hash(QByteArray(Password.toStdString().c_str()), QCryptographicHash::Sha1)).toHex();

        QSqlQuery Loginquery;
        if (!Loginquery.exec(QString("SELECT * FROM usuarios WHERE id=")+QString("'")+UserID+QString("'")))
        {
            QMessageBox::critical(0, QObject::tr("Error"),
            "No se ha podido iniciar sesión, revise sus datos");

            INICIOOK= false;
        }
        else
        {
            Loginquery.first();

            // Ahora evaluemos que las claves coinciden
            if (Loginquery.value(0)!=Encryptedpassword)
            {
                QMessageBox::critical(0, QObject::tr("Error"),
                "No se ha podido iniciar sesión, revise sus datos");

                INICIOOK= false;
            }
            else
            {
                QMessageBox::critical(0, QObject::tr("Inicio de Sesión"),
                "Bienvenido " + UserID);

                INICIOOK= true;
            }
        }
    }

    DBConnector.close();
    DBConnector.removeDatabase("QODBC");

    return INICIOOK;
}

//Esto se activara cada vez que se presione el boton de inicio de sesion
void SessionWindow::on_SessionSubmitButton_clicked()
{
    if (InicioSesion (ui->UserInput->text(), ui->PasswordInput->text()))
    {
        this->hide();
        w= new MainWindow(this);

        connect (w, SIGNAL(CerrarSesion()), this, SLOT(CerrarSesion()));
        w->show();
    }
}

//Esto se ejecutara cada vez que cierren sesion desde la ventana w
void SessionWindow::CerrarSesion()
{
    delete w;
    this->show();
}
