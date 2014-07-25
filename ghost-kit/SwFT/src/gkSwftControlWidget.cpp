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

void gkSwftControlWidget::setFunctionName(QString name)
{
    ui->functionButton->setText(name);
}

void gkSwftControlWidget::disableMoreButton()
{
    ui->functionOptions->setDisabled(true);
}

void gkSwftControlWidget::enableMoreButton()
{
    ui->functionOptions->setEnabled(true);

}

void gkSwftControlWidget::hideMoreButton()
{
    ui->functionOptions->hide();
}

void gkSwftControlWidget::showMoreButton()
{
    ui->functionOptions->show();

}
