#include "gk_dropDownBoxWidget.h"
#include "ui_gk_dropDownBoxWidget.h"

gk_dropDownBoxWidget::gk_dropDownBoxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gk_dropDownBoxWidget)
{
    ui->setupUi(this);
}

gk_dropDownBoxWidget::~gk_dropDownBoxWidget()
{
    delete ui;
}
