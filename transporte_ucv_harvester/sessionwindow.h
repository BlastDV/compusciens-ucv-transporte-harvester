#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QWidget>

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
};

#endif // SESSIONWINDOW_H
