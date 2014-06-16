#ifndef LOGREPORTER_H
#define LOGREPORTER_H

#include <QWidget>

namespace Ui
{
    class LogReporter;
}

class LogReporter : public QWidget
{
    Q_OBJECT
    
public:
    explicit LogReporter(QWidget *parent = 0);
    ~LogReporter();

    QString UserID;
    QString ConnectionName;
    
private:
    Ui::LogReporter *ui;

protected:


};

#endif // LOGREPORTER_H
