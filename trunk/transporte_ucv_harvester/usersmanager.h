#ifndef USERSMANAGER_H
#define USERSMANAGER_H

#include <QMainWindow>

namespace Ui {
class UsersManager;
}

class UsersManager : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit UsersManager(QWidget *parent = 0);
    ~UsersManager();
    
private:
    Ui::UsersManager *ui;
};

#endif // USERSMANAGER_H
