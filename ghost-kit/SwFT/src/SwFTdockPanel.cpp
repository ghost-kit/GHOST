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
void SwFTdockPanel::hideHeliospherParms()
{
    ui->CMECone->hide();
    ui->CMEConeLabel->hide();
    ui->CMELat->hide();
    ui->CMELatLabel->hide();
    ui->CMELon->hide();
    ui->CMELonLabel->hide();
    ui->CMEStartTime->hide();
    ui->CMEStartTimeLabel->hide();
    ui->CMEVel->hide();
    ui->CMEVelLabel->hide();
}

//===============================================//
void SwFTdockPanel::showHeliospherParms()
{
    ui->CMECone->show();
    ui->CMEConeLabel->show();
    ui->CMELat->show();
    ui->CMELatLabel->show();
    ui->CMELon->show();
    ui->CMELonLabel->show();
    ui->CMEStartTime->show();
    ui->CMEStartTimeLabel->show();
    ui->CMEVel->show();
    ui->CMEVelLabel->show();
}

//===============================================//
void SwFTdockPanel::constructor()
{
    this->setWindowTitle("Space Weather Forecasting Toolkit");
    QWidget* t_widget = new QWidget(this);

    this->ui->setupUi(t_widget);
    this->setWidget(t_widget);

    connect(this->ui->loadModel, SIGNAL(filenameChanged(QString)), this, SLOT(updateModelDirectory(QString)));
    connect(this->ui->Process, SIGNAL(pressed()), this, SLOT(processModel()));
    connect(this, SIGNAL(processScriptRun(bool)), this, SLOT(processScriptDone(bool)));

    this->processed = false;

    //hide Control File Info until needed
    hideHeliospherParms();
    this->controlFile = NULL;

    //start python manager
    this->pythonInterp = vtkPythonInterpreter::New();

    //attatch script test
    this->attatchMasterScript("");

    //populate the Actions list test
    this->populateActions();
}

//===============================================//
SwFTdockPanel::~SwFTdockPanel()
{
    delete ui;
    this->pythonInterp->Delete();
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
void SwFTdockPanel::populateActions()
{
    QGroupBox *ControlGrouping = new QGroupBox;
    QGridLayout *CGlayout = new QGridLayout;

    //this is where actions will be defined.
    this->addAnalysisFunction("Reset All Views", "void", CGlayout);

    ui->spaceWeatherControls->setLayout(CGlayout);
}

//===============================================//
void SwFTdockPanel::updateModelDirectory(QString modelSourceDir)
{
    std::cout << __FUNCTION__ << " Has Fired" << std::endl;
    this->fileList = ui->loadModel->filenames();

    //extract the directory of the model:
    QStringList filePath = this->fileList[0].split("/");
    filePath[filePath.length()-1] = "";
    QString Path = filePath.join("/");
    ui->dir->setText(filePath[filePath.length()-2]);

    this->currentDirectory = Path;


    //look for control file
    if(this->findControlFile())
    {

        ui->modelType->setText("ENLIL");
        ui->ControlFile->setText(QString("./") + this->controlFileName.split("/").last());

        QStringList CMElat;
        QStringList CMElon;
        QStringList CMEcone;
        QStringList CMEvel;
        QStringList CMEstartDate;

        //open control file
        if(this->controlFile)
        {
            delete this->controlFile;
        }

        this->controlFile = new enlilControlFile(this->controlFileName.toAscii().data());

        //populate the correct fields
        int nCME = this->controlFile->getNCME();

        for(int x = 0; x < nCME; x++)
        {
            CMElat.push_back(QString::number(this->controlFile->getCmeLatCloud(x)));
            CMElon.push_back(QString::number(this->controlFile->getCmeLonCloud(x)));
            CMEcone.push_back(QString::number(this->controlFile->getCmeRCloud(x)));
            CMEvel.push_back(QString::number(this->controlFile->getCmeVelCloud(x)));
            CMEstartDate.push_back(QString::fromStdString(this->controlFile->getCmeDateCloud(x).getDateTimeString()));
        }

        //update GUI params
        ui->CMECone->setText(CMEcone.join(";"));
        ui->CMELat->setText(CMElat.join(";"));
        ui->CMELon->setText(CMElon.join(";"));
        ui->CMEVel->setText(CMEvel.join(";"));
        ui->CMEStartTime->setText(CMEstartDate.join(";"));

        //show heliospheric info
        this->showHeliospherParms();
    }
    else
    {
        //no control file
        ui->modelType->setText("UNKNOWN");
        std::cout << "No Control File Found... not providing control information." << std::endl;

    }

    emit this->processScriptRun(false);

}

bool SwFTdockPanel::initSwFT()
{
    this->pythonInterp->SetProgramName("SwFT");

    return(this->pythonInterp->Initialize());
}

void SwFTdockPanel::processModel()
{
    std::cout << __FUNCTION__ << " Has Fired." << std::endl;
    emit this->processScriptRun(true);

    //Initialize the Python Interpreter with the program name of SwFT
    bool status = initSwFT();

    if(status)
    {
        std::cout << "SwFT initialized" << std::endl;
    }
    else
    {
        std::cout << "Python already running" << std::endl;
    }

    //test
    vtksys_ios::ostringstream stream;
    stream << "newStuf = Cone()" << endl
           << endl
           << "newStufRep = Show()" << endl;

    vtkPythonInterpreter::RunSimpleString(stream.str().c_str());


}

//===============================================//
void SwFTdockPanel::executeAnalysisCommand(QString name)
{

}

void SwFTdockPanel::processScriptDone(bool state)
{
    this->ui->Process->setDisabled(state);
}

int SwFTdockPanel::findControlFile()
{
    //Look in current directory for control file with the name of "control_file"
    QString CFPath = this->currentDirectory + "/control_file";

    //check existence
    std::ifstream file(CFPath.toAscii().data());
    if (file.good())
    {
        std::cerr << "File Located" << std::endl;

        file.close();
        this->controlFileName.clear();
        this->controlFileName = CFPath;

        return true;
    }

    std::cerr << "File Not Found " << std::endl;

    file.close();
    this->controlFileName.clear();

    return false;
}
