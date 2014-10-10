#include "devicesetting.h"
#include "ui_devicesetting.h"

DeviceSetting::DeviceSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceSetting)
{
    ui->setupUi(this);
}

DeviceSetting::~DeviceSetting()
{
    delete ui;
}
