#ifndef DEVICESETTING_H
#define DEVICESETTING_H

#include <QWidget>

#include "csp32bridge.h"
#include "logmaster.h"

namespace Ui
{
    class DeviceSetting;
}

class DeviceSetting : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceSetting(QWidget *parent = 0);
    ~DeviceSetting();

private:
    Ui::DeviceSetting *ui;
};

#endif // DEVICESETTING_H
