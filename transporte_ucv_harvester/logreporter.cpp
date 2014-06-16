#include "logreporter.h"
#include "ui_logreporter.h"

LogReporter::LogReporter(QWidget *parent) : QWidget(parent), ui(new Ui::LogReporter)
{
    ui->setupUi(this);
}

LogReporter::~LogReporter()
{
    delete ui;
}
