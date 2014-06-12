#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QWidget>
#include <QCryptographicHash>

namespace Ui {
class SessionWindow;
}

class SessionWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SessionWindow(QWidget *parent = 0);
    ~SessionWindow();

private:
    Ui::SessionWindow *ui;
    QCryptographicHash *encrypter;
};

#endif // SESSIONWINDOW_H
