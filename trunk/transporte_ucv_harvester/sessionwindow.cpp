#include "sessionwindow.h"
#include "ui_sessionwindow.h"

SessionWindow::SessionWindow(QWidget *parent) : QWidget(parent), ui(new Ui::SessionWindow)
{
    ui->setupUi(this);

}

SessionWindow::~SessionWindow()
{
    delete ui;
}
