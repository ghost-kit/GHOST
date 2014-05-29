#include "SwFTdockPanel.h"
#include "ui_SwFTdockPanel.h"
#include "SwFTdockPanel.h"

SwFTdockPanel::~SwFTdockPanel()
{
    delete ui;
}

void SwFTdockPanel::addAnalysisFunction(QString name, QString command)
{

}

void SwFTdockPanel::removeAnalysisFunction(QString name)
{

}

void SwFTdockPanel::attatchMasterScript(QString script)
{

}

void SwFTdockPanel::constructor()
{
    this->setWindowTitle("Space Weather Forecasting Toolkit");
    QWidget* t_widget = new QWidget(this);
    Ui::SwFTdockPanel ui;
    ui.setupUi(t_widget);
    this->setWidget(t_widget);
}

void SwFTdockPanel::initializePython()
{

}

void SwFTdockPanel::updateModelDirectory(QString modelSourceDir)
{

}

void SwFTdockPanel::executeAnalysisCommand(QString name)
{

}
