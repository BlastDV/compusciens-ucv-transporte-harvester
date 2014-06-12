#include "sessionwindow.h"
#include "ui_sessionwindow.h"
#include "stdio.h"
#include <QMessageBox>

SessionWindow::SessionWindow(QWidget *parent) : QWidget(parent), ui(new Ui::SessionWindow)
{
    ui->setupUi(this);

    QByteArray result= encrypter->hash("admin", QCryptographicHash::Sha1);

    QMessageBox::critical(0, QObject::tr("Su palabra encriptada"),
                          QString(result.toHex()));
}

SessionWindow::~SessionWindow()
{
    delete ui;
}
