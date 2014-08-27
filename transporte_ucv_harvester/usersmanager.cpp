#include "usersmanager.h"
#include "ui_usersmanager.h"

UsersManager::UsersManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UsersManager)
{
    ui->setupUi(this);
}

UsersManager::~UsersManager()
{
    delete ui;
}
