#include "gkSwftControlWidget.h"
#include "ui_gkSwftControlWidget.h"

gkSwftControlWidget::gkSwftControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gkSwftControlWidget)
{
    ui->setupUi(this);
}

gkSwftControlWidget::~gkSwftControlWidget()
{
    delete ui;
}
