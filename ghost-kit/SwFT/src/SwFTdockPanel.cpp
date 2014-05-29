#include "SwFTdockPanel.h"
#include "ui_SwFTdockPanel.h"
#include "SwFTdockPanel.h"
#include <iostream>
#include <pqFileChooserWidget.h>

//===============================================//
SwFTdockPanel::SwFTdockPanel(const QString &t, QWidget *p, Qt::WindowFlags f)
    : Superclass(t, p, f),
       ui(new Ui::SwFTdockPanel)
{
    this->constructor();
}

//===============================================//
SwFTdockPanel::SwFTdockPanel(QWidget *p, Qt::WindowFlags f)
    : Superclass(p, f),
       ui(new Ui::SwFTdockPanel)
{
    this->constructor();
}

//===============================================//
void SwFTdockPanel::constructor()
{
    this->setWindowTitle("Space Weather Forecasting Toolkit");
    QWidget* t_widget = new QWidget(this);

    this->ui->setupUi(t_widget);
    this->setWidget(t_widget);

    connect(this->ui->loadModel, SIGNAL(filenameChanged(QString)), this, SLOT(updateModelDirectory(QString)));



    //start python manager
    this->pythonManager = new pqPythonManager();

    //attatch script test
    this->attatchMasterScript("");

    //populate the Actions list test
    this->populateActions();
}

//===============================================//
SwFTdockPanel::~SwFTdockPanel()
{
    delete ui;
    delete this->pythonManager;
}

//===============================================//
void SwFTdockPanel::addAnalysisFunction(QString name, QString command, QLayout* layout)
{
    std::cerr << "working on adding Analsysis functions" << std::endl;

    //create widget
    gkSwftControlWidget* n_widget = new gkSwftControlWidget;

    //setup widget
    n_widget->setFunctionName(name);
    n_widget->hideMoreButton();

    //add to layout
    layout->addWidget(n_widget);

    //map command
    this->commandMap[name] = command;


}

//===============================================//
void SwFTdockPanel::removeAnalysisFunction(QString name)
{

}

//===============================================//
void SwFTdockPanel::attatchMasterScript(QString script)
{
    //This is where the actual code for the analysis is attatched
    std::cerr << "No code has yet been attatched" << std::endl;
}



//===============================================//
void SwFTdockPanel::initializePython()
{

}

//===============================================//
void SwFTdockPanel::populateActions()
{
    QGroupBox *ControlGrouping = new QGroupBox;
    QGridLayout *CGlayout = new QGridLayout;

    //this is where actions will be defined.
    this->addAnalysisFunction("Reset All Views", "void", CGlayout);

    ui->modelType->setText("Enlil");
    ui->spaceWeatherControls->setLayout(CGlayout);
}

//===============================================//
void SwFTdockPanel::updateModelDirectory(QString modelSourceDir)
{
    std::cerr << __FUNCTION__ << " Has Fired" << std::endl;
}

//===============================================//
void SwFTdockPanel::executeAnalysisCommand(QString name)
{

}
