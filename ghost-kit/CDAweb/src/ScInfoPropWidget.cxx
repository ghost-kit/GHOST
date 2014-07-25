#include "ScInfoPropWidget.h"
#include "ui_ScInfoPropWidget.h"
#include "ltrDateTime.h"
#include "pqPropertiesPanel.h"
#include "pqTreeWidgetItem.h"
#include "pqTreeWidgetSelectionHelper.h"
#include "pqTreeWidgetCheckHelper.h"
#include "pqTreeWidgetItemObject.h"
#include "pqSelectionInspectorWidget.h"
#include "pqSelectionManager.h"
#include "pqArrayListDomain.h"
#include "pqApplicationCore.h"
#include "pqTreeWidget.h"
#include "filterNetworkAccessModule.h"
#include <QListWidgetItem>
#include "qlist.h"
#include "QStringList"
#include "qstring.h"
#include <vtkSMProxy.h>
#include <vtkSMProperty.h>
#include <vtkSMSessionProxyManager.h>
#include <vtkSMGlobalPropertiesManager.h>
#include <vtkSMStringVectorProperty.h>
#include <vtkSMDoubleVectorProperty.h>
#include <iomanip>
#include <Qt>
//==================================================================
void ScInfoPropWidget::setGroupGUI()
{
    filterNetworkAccessModule SCListManager;
    this->getSCList(SCListManager);
    this->currentGroupObjects = SCListManager.getFinalOjects();

    //get the observatory group list
    this->getGroupsList();

    //Load the group list
    this->loadGroupListToGUI();

    //all variable setup
    this->getAllVars = true;
    ui->allVariables->setChecked(true);
    ui->Variables->setVisible(false);
    ui->Variables_Label->setVisible(false);
}

void ScInfoPropWidget::restoreGUIinit()
{
    std::cerr << "WARNING: Restore from State not yet FULLY implemented" << std::endl;

    //clean auto-restored junk
    this->InstrumentSelectionTracker->RemoveAllArrays();
    this->ObservatorySelectionTracker->RemoveAllArrays();

    int Dsize = this->DataSetSelectionTracker.size();
    QStringList keys = this->DataSetSelectionTracker.keys();
    for(int x = 0; x < Dsize; x ++)
    {
        this->DataSetSelectionTracker[keys[x]]->RemoveAllArrays();
    }

    int Vsize = this->VariablesSelectionTracker.size();
    QStringList xKeys = this->VariablesSelectionTracker.keys();
    for(int x = 0; x < Vsize; x++)
    {
        int Dsize = this->VariablesSelectionTracker[xKeys[x]].size();

        QStringList vKeys = this->VariablesSelectionTracker.keys();
        for(int y = 0; y < Dsize; y++)
        {
            this->VariablesSelectionTracker[xKeys[x]][vKeys[y]]->RemoveAllArrays();
        }
    }

    //Restore the Group GUI list
    this->setGroupGUI();

    //Restoring State of the Group GUI
    QString activeItem = QString(this->SaveStateGroup->GetElement(0));
    int activeIndex = ui->Group->findText(activeItem);
    ui->Group->setCurrentIndex(activeIndex);
    this->currentGroup = activeItem;

    //Restore Observatories
    this->getObservatoryList(activeItem);
    this->setObservatoryGUI();

    //Restore Active Observatory
    activeItem = QString(this->SaveStateObservatory->GetElement(0));
    activeIndex = ui->Observatory->findText(activeItem);
    ui->Observatory->setCurrentIndex(activeIndex);
    this->currentObservatory = activeItem;

    //Restore Instruments
    this->InstrumentSelectionTracker->RemoveAllArrays();
    this->setInstumentGUI();

    //Restore Active Instruments
    int numInsts = this->SaveStateInstrument->GetNumberOfElements();
    for(int x = 0; x < numInsts; x++)
    {
        activeItem = QString(this->SaveStateInstrument->GetElement(x));
        QList<QTreeWidgetItem *> activeItemList = ui->Instruments->findItems(activeItem, Qt::MatchExactly | Qt::MatchRecursive );
        if(activeItemList.size() > 0)
        {
            activeItemList[0]->setCheckState(0, Qt::Checked);
        }
    }
    //Restore DataSets
    this->updateDataSet();
    this->updateVariables();

    //Restore Active DataSets
    std::cout << "DataSet Format: " << this->SaveStateDataSet->GetElement(0) << std::endl;
    int numDS = this->SaveStateDataSet->GetNumberOfElements();
    QMap<QString, QStringList> DataSetRestore;

    for(int x = 0; x < numDS; x++)
    {
        QStringList breakdown = QString(this->SaveStateDataSet->GetElement(x)).split("::");
        if(breakdown.size() > 1)
        {
            DataSetRestore[breakdown[0]].push_back(breakdown[1]);
        }
        else
        {
            std::cerr << "Failure Restoring State..." << std::endl;
        }
    }

    //Activate the items...
    QStringList DKeys = DataSetRestore.keys();
    for(int x = 0; x < DataSetRestore.size(); x++)
    {

        QStringList activeItems = DataSetRestore[DKeys[x]];
        for(int y = 0; y < activeItems.size(); y++)
        {
            QList<QTreeWidgetItem*> activeItemList = ui->DataSet->findItems(activeItems[y], Qt::MatchExactly | Qt::MatchRecursive );
            if(activeItemList.size() > 0)
            {
                activeItemList[0]->setCheckState(0, Qt::Checked);
            }
            else
            {
                std::cerr << "Error Restoring State" << std::endl;
            }
        }
    }

    //TODO: Restore Variables
    //TODO: Restore Active Variables

}

ScInfoPropWidget::ScInfoPropWidget(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject)
    : Superclass(smproxy, parentObject),
      ui(new Ui::ScInfoPropWidget)
{

    this->smProperty = smproperty;
    this->smProxy = smproxy;

    //dont show label
    this->setShowLabel(false);

    //make save state selection properties available
    this->SaveStateGroup       = vtkSMStringVectorProperty::SafeDownCast(this->smProxy->GetProperty("Group"));
    this->SaveStateObservatory = vtkSMStringVectorProperty::SafeDownCast(this->smProxy->GetProperty("Observatory"));
    this->SaveStateInstrument  = vtkSMStringVectorProperty::SafeDownCast(this->smProxy->GetProperty("Instruments"));
    this->SaveStateDataSet     = vtkSMStringVectorProperty::SafeDownCast(this->smProxy->GetProperty("DataSets"));
    this->SaveStateVariables   = vtkSMStringVectorProperty::SafeDownCast(this->smProxy->GetProperty("Variables"));

    //intiailize the time state
    this->startTimeState            = vtkSMDoubleVectorProperty::SafeDownCast(this->smProxy->GetProperty("TimeStart"));
    this->endTimeState              = vtkSMDoubleVectorProperty::SafeDownCast(this->smProxy->GetProperty("TimeEnd"));

    //===============================================================================================
    //DEBUG//
    if(this->SaveStateGroup->GetNumberOfElements() > 0)
        std::cout << "Group:       " << this->SaveStateGroup->GetElement(0) << std::endl;

    if(this->SaveStateObservatory->GetNumberOfElements() > 0)
        std::cout << "Observatory: " << this->SaveStateObservatory->GetElement(0) << std::endl;

    if(this->SaveStateInstrument->GetNumberOfElements() > 0)
    {
        for(int x = 0; x < this->SaveStateInstrument->GetNumberOfElements(); x++)
        {
            std::cout << "Instrument:  " << this->SaveStateInstrument->GetElement(x) << std::endl;
        }
    }

    if(this->SaveStateDataSet->GetNumberOfElements() > 0)
    {
        for(int x = 0; x < this->SaveStateDataSet->GetNumberOfElements(); x++)
        {
            std::cout << "DataSet:     " << this->SaveStateDataSet->GetElement(x) << std::endl;
        }
    }

    if(this->SaveStateVariables->GetNumberOfElements() > 0)
    {
        for(int x = 0; x < this->SaveStateVariables->GetNumberOfElements(); x++)
        {
            std::cout << "Variable:    " << this->SaveStateVariables->GetElement(x) << std::endl;
        }
    }



    //END DEBUG//
    //===============================================================================================

    //make return property available
    this->svp = vtkSMStringVectorProperty::SafeDownCast(smproperty);
    if(!svp)
    {
        return;
    }

    //get properties
    this->startMJD = this->startTimeState->GetElement(0);
    this->endMJD = this->endTimeState->GetElement(0);

    //URLs for CDAWeb
    this->baseURL = QString("http://cdaweb.gsfc.nasa.gov/WS/cdasr/1");
    this->dataViewSpacePhys = QString("/dataviews/sp_phys/");
    this->getObservatorys = QString("observatories");
    this->getObservatoryGroups = QString("observatoryGroups");
    this->getInstrumentTypes = QString("instrumentTypes");
    this->getInstruments = QString("instruments");
    this->getDataSets = QString("datasets");
    this->getDataGroups=QString("datasets");
    this->getVariables=QString("variables");
    this->getInventory=QString("inventory");

    ui->setupUi(this);
    ui->gridLayout->setMargin(pqPropertiesPanel::suggestedMargin());
    ui->gridLayout->setHorizontalSpacing(pqPropertiesPanel::suggestedHorizontalSpacing());
    ui->gridLayout->setVerticalSpacing(pqPropertiesPanel::suggestedVerticalSpacing());

    if(!smproxy->GetProperty("TimeRange"))
    {
        ui->startTime->hide();
        ui->startTime_Label->hide();
        ui->endTime->hide();
        ui->endTime_Label->hide();
    }

    //Initialize trackers
    this->GroupSelectionTracker             = vtkDataArraySelection::New();
    this->ObservatorySelectionTracker       = vtkDataArraySelection::New();
    this->InstrumentSelectionTracker        = vtkDataArraySelection::New();
    this->InstrumentDataSetInfoCacheStatus  = vtkDataArraySelection::New();

    //connect signals to slots
    /** Time Connections */
    connect(ui->startTime, SIGNAL(editingFinished()), this, SLOT(timeRangeChanged()));
    connect(ui->startTime, SIGNAL(editingFinished()), this, SLOT(startTimeChanged()));
    connect(ui->endTime, SIGNAL(editingFinished()), this, SLOT(timeRangeChanged()));
    connect(ui->endTime, SIGNAL(editingFinished()), this, SLOT(endTimeChanged()));

    /** Group Connections */
    connect(ui->Group, SIGNAL(activated(QString)), this, SLOT(groupSelectedChanged(QString)));

    /** Observatory Connections */
    connect(ui->Observatory, SIGNAL(activated(QString)), this, SLOT(observatorySelectionChanged(QString)));

    /** Instrument Connections/Links */
    this->connect(ui->Instruments, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(instrumentSelectionChanged(QTreeWidgetItem*,int)));
    this->addPropertyLink(ui->Instruments, smproxy->GetPropertyName(smproperty), SIGNAL(itemChanged(QTreeWidgetItem*,int)), this->svp);

    /** Data Selection Connections/Links */
    connect(ui->DataSet, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(dataSetSelectionChanged(QTreeWidgetItem*,int)));
    this->addPropertyLink(ui->DataSet, smproxy->GetPropertyName(smproperty), SIGNAL(itemChanged(QTreeWidgetItem*,int)), this->svp);

    /** Variable connections/links*/
    this->connect(ui->Variables, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(variableSelectionChanged(QTreeWidgetItem*,int)));
    this->connect(ui->allVariables, SIGNAL(clicked(bool)), this, SLOT(useAllVariables(bool)));
    this->addPropertyLink(ui->Variables, smproxy->GetPropertyName(smproperty), SIGNAL(itemChanged(QTreeWidgetItem*,int)), this->svp);
    this->addPropertyLink(ui->allVariables, smproxy->GetPropertyName(smproperty), SIGNAL(clicked()), this->svp);


    //Load first set of Values
    if(QString(this->SaveStateGroup->GetElement(0)) == "")
    {
        this->setGroupGUI();
    }
    else //restore state
    {
        this->restoreGUIinit();
    }



}


//==================================================================
ScInfoPropWidget::~ScInfoPropWidget()
{

    delete ui;
}

//==================================================================
void ScInfoPropWidget::apply()
{
    std::cout << "APPLY CLICKED" << std::endl;

    //gather the requisite selection information
    QMap<QString, QMap<QString, QStringList> > selectionMap;
    QStringList Instruments = this->DataSetSelectionTracker.keys();
    QString CodeString;

    int instCount = 0;
    int VARcount = 0;
    int DScount = 0;

    //clear out the old values
    this->cleanStateProperties();

    QString DataSetSaveState;
    QString VariableSaveState;

    for(int i = 0; i < Instruments.size(); i++)
    {
        //skip if instruemnt is not active
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(Instruments[i].toAscii().data())) continue;

        //save the state of the instruments
        this->SaveStateInstrument->SetElement(instCount, Instruments[i].toAscii().data());
        std::cout << "Saved Instrument: " << this->SaveStateInstrument->GetElement(instCount) << std::endl;

        //move on to data sets within the Instrument
        for(int d = 0; d < DataSetSelectionTracker[Instruments[i]]->GetNumberOfArrays(); d++)
        {
            //get data set name
            QString activeDataSet = this->DataSetSelectionTracker[Instruments[i]]->GetArrayName(d);

            //skip if the dataset is not active
            if(!this->DataSetSelectionTracker[Instruments[i]]->ArrayIsEnabled(activeDataSet.toAscii().data())) continue;

            //clear last Instrument State
            DataSetSaveState = Instruments[i] + "::" + activeDataSet;
            this->SaveStateDataSet->SetElement(DScount, DataSetSaveState.toAscii().data());
            std::cout << "Saved DataSet: " << this->SaveStateDataSet->GetElement(DScount) << std::endl;

            //otherwise, move on to the Variabels
            if(this->getAllVars)
            {
                //Save the State
                this->SaveStateVariables->SetNumberOfElements(1);
                VariableSaveState = "ALL";
                this->SaveStateVariables->SetElement(0, VariableSaveState.toAscii().data());
                std::cout << "Saved Variable: " << this->SaveStateVariables->GetElement(0) << std::endl;

                VARcount ++;

                //get info for return string
                for(int v = 0; v < this->VariableSetInformation[Instruments[i]][activeDataSet].size(); v++)
                {
                    //get variable ID
                    QString varName = this->VariableSetInformation[Instruments[i]][activeDataSet][v].ID;

                    //add variable to the map
                    selectionMap[Instruments[i]][activeDataSet].push_back(varName);
                }

            }
            else
            {
                for(int v = 0; v < VariablesSelectionTracker[Instruments[i]][activeDataSet]->GetNumberOfArrays(); v++)
                {
                    //get variable name
                    QString varName = VariablesSelectionTracker[Instruments[i]][activeDataSet]->GetArrayName(v);

                    //skip if the variable is not active
                    if(!this->VariablesSelectionTracker[Instruments[i]][activeDataSet]->ArrayIsEnabled(varName.toAscii().data())) continue;

                    //Save the State
                    VariableSaveState = DataSetSaveState + QString(";;") + varName;
                    this->SaveStateVariables->SetElement(VARcount, VariableSaveState.toAscii().data());
                    std::cout << VARcount << "-Saved Variable: " << this->SaveStateVariables->GetElement(VARcount) << std::endl;

                    //add variable to the map
                    selectionMap[Instruments[i]][activeDataSet].push_back(varName);

                    //increment the state counter
                    VARcount ++;
                }
            }

            DScount++;
        }

        //advance counter
        instCount++;
    }


    //build the code string//
    /////////////////////////
    QStringList selectedInstruments = selectionMap.keys();
    int instrumentSize = selectedInstruments.size();

    for(int x = 0; x < instrumentSize; x++)
    {
        if(x != 0)
        {
            CodeString = CodeString + ";";
        }

        //add current instrument
        CodeString = CodeString + selectedInstruments[x] + ":";

        //get list of DataSets
        QStringList selectedDataSets = selectionMap[selectedInstruments[x]].keys();

        //Number of datasets
        int datasetSize = selectedDataSets.size();

        for(int y = 0; y < datasetSize; y++)
        {
            if(y != 0)
            {
                CodeString = CodeString + ",";
            }

            //add current dataset
            CodeString = CodeString + selectedDataSets[y] + "~";

            //list of variables
            QStringList selectedVariables = selectionMap[selectedInstruments[x]][selectedDataSets[y]];

            //number of variables
            int variableSize = selectedVariables.size();

            //process variables
            for(int z = 0; z < variableSize; z++)
            {
                if(z != 0)
                {
                    CodeString = CodeString + "|";
                }

                //add variable
                CodeString = CodeString + selectedVariables[z];

            }

        }
    }

    std::cout << "Code String: " << CodeString.toAscii().data() << std::endl;

    //Add relevent information to ParaView Property
    this->svp->SetElement(0, QString::number(this->startMJD, 'g', 17).toAscii().data());
    this->svp->SetElement(1, QString::number(this->endMJD, 'g', 17).toAscii().data());

    this->svp->SetElement(2, this->currentGroup.toAscii().data());
    this->SaveStateGroup->SetElement(0, this->currentGroup.toAscii().data());

    this->svp->SetElement(3, this->currentObservatory.toAscii().data());
    this->SaveStateObservatory->SetElement(0, this->currentObservatory.toAscii().data());

    this->svp->SetElement(4, CodeString.toAscii().data());

    //Time Range Stuff...
    if(this->smProxy->GetProperty("TimeRange"))
    {
        //set date and time
        QDateTime start = ui->startTime->dateTime();

        DateTime startDT;
        startDT.setYear(start.date().year());
        startDT.setMonth(start.date().month());
        startDT.setDay(start.date().day());

        startDT.setHours(start.time().hour());
        startDT.setMinutes(start.time().minute());
        startDT.setSeconds(start.time().second());

        QDateTime end = ui->endTime->dateTime();

        DateTime endDT;
        endDT.setYear(end.date().year());
        endDT.setMonth(end.date().month());
        endDT.setDay(end.date().day());

        endDT.setHours(end.time().hour());
        endDT.setMinutes(end.time().minute());
        endDT.setSeconds(end.time().second());

        vtkSMDoubleVectorProperty *timeRange =  vtkSMDoubleVectorProperty::SafeDownCast(this->smProxy->GetProperty("TimeRange"));
        timeRange->SetElement(0,startDT.getMJD());
        timeRange->SetElement(1,endDT.getMJD());
    }

    //apply the upstream parameters
    Superclass::apply();

}

void ScInfoPropWidget::reset()
{
//    this->restoreGUIinit();

    Superclass::reset();
}

//==================================================================
bool ScInfoPropWidget::getSCList(filterNetworkAccessModule &manager)
{
    //get data from network
    manager.Get(this->baseURL + this->dataViewSpacePhys + this->getObservatoryGroups,
                QString("ObservatoryGroups"),
                QString("ObservatoryGroupDescription"));

    return true;

}

//==================================================================
bool ScInfoPropWidget::getSCInstrument(filterNetworkAccessModule &manager)
{
    manager.Get(this->baseURL + this->dataViewSpacePhys + this->getInstruments
                + "?observatory=" + this->currentObservatory,
                QString("Instruments"),
                QString("InstrumentDescription"));
    return true;

}

//==================================================================
bool ScInfoPropWidget::getSciDataGroup(filterNetworkAccessModule &manager, QString dataset)
{
    manager.Get(this->baseURL + this->dataViewSpacePhys + this->getDataSets + "?observatoryGroup="
                + this->currentGroup + "&observatory=" + this->currentObservatory + "&instrument=" + dataset ,
                QString("Datasets"),
                QString("DatasetDescription"));

    return true;

}

//==================================================================
bool ScInfoPropWidget::getSciVariables(filterNetworkAccessModule &manager, QString dataset)
{
    manager.Get(this->baseURL + this->dataViewSpacePhys + this->getDataSets + "/" + dataset + "/" + this->getVariables,
                QString("Variables"),
                QString("VariableDescription"));
    return true;
}

//==================================================================
//parse for groups
bool ScInfoPropWidget::getGroupsList()
{
    //get new group information
    for(int x = 0; x < this->currentGroupObjects->size(); x++)
    {
        //get name
        filterNetworkObject *currentMap = this->currentGroupObjects->operator [](x);
        QString name = currentMap->operator []("Name");
        this->ObsGroupList.push_back(name);
    }

    ObsGroupList.sort();

    return true;
}


//==================================================================
//parse Observatory List
bool ScInfoPropWidget::getObservatoryList(QString Group)
{
    //clear what is there already
    this->ObservatoryList.clear();

    //find the ObservatoryId
    for(int x = 0; x < this->currentGroupObjects->size(); x++)
    {
        filterNetworkObject *currentMap = this->currentGroupObjects->operator [](x);

        if(currentMap->operator []("Name") == Group)
        {
            this->ObservatoryList = currentMap->values("ObservatoryId");
        }
    }

    this->ObservatoryList.sort();

    return true;
}

//==================================================================
//load the group list to the GUI
bool ScInfoPropWidget::loadGroupListToGUI()
{
    ui->Group->clear();
    ui->Group->addItem("--- Select Group ---");
    ui->Group->addItems(this->ObsGroupList);

    return true;
}

//==================================================================
//process the change in selection on the selection box
void ScInfoPropWidget::setObservatoryGUI()
{

    ui->Observatory->clear();
    ui->Observatory->addItem("--- Select Observatory ---");
    ui->Observatory->addItems(this->ObservatoryList);

}

void ScInfoPropWidget::groupSelectedChanged(QString selection)
{
    this->currentGroup = selection;
    this->currentObservatory = "";

    if(selection != "--- Select Group ---")
    {
        this->getObservatoryList(selection);
        this->setObservatoryGUI();
    }
    else
    {
        ui->Observatory->clear();
    }
    //clear the downstream UI states
    ui->Instruments->clear();
    ui->Variables->clear();
    ui->DataSet->clear();

}

//==================================================================
//process Observatories
void ScInfoPropWidget::setInstumentGUI()
{
    filterNetworkAccessModule SCInstrumentManager;

    //get data for next item
    this->getSCInstrument(SCInstrumentManager);
    this->currentInstrumentObjects = SCInstrumentManager.getFinalOjects();

    //get the required list
    this->getInstrumentList();

    //configure the gui for display
    ui->Instruments->clear();
    ui->Variables->clear();
    ui->DataSet->clear();
    ui->Instruments->setRootIsDecorated(false);
    ui->Instruments->setColumnCount(2);

    QStringList InstrumentNames = this->InstrumentList.keys();
    QStringList InstrumentDesc  = this->InstrumentList.values();

    // populate the tree
    for(int x = 0; x < this->InstrumentList.size(); x++)
    {

        //lets not get instruments with no descriptiong
        //  This descision can easily be reversed
        if(InstrumentDesc[x] != "")
        {

            //populate the tracker
            this->InstrumentSelectionTracker->AddArray(InstrumentNames[x].toAscii().data());
            this->InstrumentSelectionTracker->DisableArray(InstrumentNames[x].toAscii().data());

            this->InstrumentDataSetInfoCacheStatus->AddArray(InstrumentNames[x].toAscii().data());
            this->InstrumentDataSetInfoCacheStatus->DisableArray(InstrumentNames[x].toAscii().data());

            //add new tracker to the DataSet map
            this->DataSetSelectionTracker[InstrumentNames[x]] = vtkDataArraySelection::New();
            this->DataSetVariableInfoCacheStatus[InstrumentNames[x]] = vtkDataArraySelection::New();

            //populate the GUI tree items
            pqTreeWidgetItem *newItem = new pqTreeWidgetItem();

            newItem->setText(0,InstrumentNames[x] );
            newItem->setText(1,InstrumentDesc[x] );
            newItem->setTextColor(0,QColor("Dark Blue"));
            newItem->setCheckState(0,Qt::Unchecked);

            //place in the GUI
            ui->Instruments->addTopLevelItem(newItem);

        }
    }
}

void ScInfoPropWidget::observatorySelectionChanged(QString selection)
{

    //clear the Instrument Tracker
    this->InstrumentSelectionTracker->RemoveAllArrays();

    //clear DataSet and Variables
    this->DataSetSelectionTracker.clear();
    this->VariablesSelectionTracker.clear();

    //set the observatory selection information
    this->currentObservatory = selection;

    //Populate the Instrument GUI
    this->setInstumentGUI();
}


//==================================================================
//Instrument Selection has Changed
void ScInfoPropWidget::getAllDataSetInfo()
{

    std::cout << "Number of Instrument Arrays:        " << this->InstrumentSelectionTracker->GetNumberOfArrays() << std::endl;
    std::cout << "Number of Active Instrument Arrays: " << this->InstrumentSelectionTracker->GetNumberOfArraysEnabled() << std::endl;

    for(int x = 0; x < this->InstrumentSelectionTracker->GetNumberOfArrays(); x ++)
    {
        //get name of current array
        QString NameOfArray = QString(this->InstrumentSelectionTracker->GetArrayName(x));

        //check to see if the data is selected
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(NameOfArray.toAscii().data())) continue;

        //if the data is already in the cache, we don't need to download it
        if(this->InstrumentDataSetInfoCacheStatus->ArrayIsEnabled(NameOfArray.toAscii().data())) continue;

        //Download the data
        filterNetworkAccessModule SCDataSetListManager;
        this->getSciDataGroup(SCDataSetListManager, NameOfArray);

        std::cout << "Data Downloaded for: " << NameOfArray.toAscii().data() << std::endl;

        //Cache the Data
        this->InstrumentDataSetInfoCache[NameOfArray] = SCDataSetListManager.getFinalOjects();
        this->InstrumentDataSetInfoCacheStatus->EnableArray(NameOfArray.toAscii().data());

    }
}

//==================================================================
void ScInfoPropWidget::getAllVariableSetInfo()
{
    //get the keys that exist
    QStringList Instruments = this->DataSetSelectionTracker.keys();
    for(int x =0; x < Instruments.size(); x++)
    {
        if(this->InstrumentSelectionTracker->ArrayIsEnabled(Instruments[x].toAscii().data()))
        {
            //get keys of data sets
            int numberArrays = this->DataSetSelectionTracker[Instruments[x]]->GetNumberOfArrays();

            for(int y =0; y < numberArrays; y++)
            {

                //get information on the Variables in DataSets
                QString InstrumentName = Instruments[x];
                QString DataSetName    = DataSetSelectionTracker[Instruments[x]]->GetArrayName(y);

                //check to see if the data is selected
                if(!this->DataSetSelectionTracker[Instruments[x]]->ArrayIsEnabled(DataSetName.toAscii().data())) continue;

                //if the data is already in the cache, we don't need to download it
                if(this->DataSetVariableInfoCacheStatus[InstrumentName]->ArrayIsEnabled(DataSetName.toAscii().data())) continue;

                //Download the Data
                filterNetworkAccessModule SCVariableListManager;
                this->getSciVariables(SCVariableListManager, DataSetName);

                //Cache the Data
                this->DataSetVariableInfoCache[InstrumentName][DataSetName] = SCVariableListManager.getFinalOjects();
                this->DataSetVariableInfoCacheStatus[InstrumentName]->EnableArray(DataSetName.toAscii().data());
            }
        }
    }
}

//==================================================================
void ScInfoPropWidget::extractDataSetInfo()
{
    //This method extracts needed information from the XML
    for(int x = 0; x < this->InstrumentSelectionTracker->GetNumberOfArrays(); x ++)
    {
        QString NameOfArray = QString(InstrumentSelectionTracker->GetArrayName(x));

        //skip if not enabled
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(NameOfArray.toAscii().data())) continue;

        //process the enabled arrays
        //get the required data object
        filterNetworkList *item = this->InstrumentDataSetInfoCache[NameOfArray];

        if(!item)
        {
            std::cerr << "ERROR: Invalid Pointer" << std::endl;
            break;
        }

        //skip processing if we have already processed it.
        if(this->DataSetInformation[NameOfArray].size() > 0) continue;

        //process the data
        for(int y = 0; y < item->size(); y++)
        {
            //get the current map
            filterNetworkObject *currentMap = item->operator [](y);
            DataSetInfo newInfoObject;

            //extract the needed information from the map and place in a DataSetInformation object
            newInfoObject.Name = currentMap->operator []("Label");
            newInfoObject.ID = currentMap->operator []("Id");
            newInfoObject.ObsGroup = currentMap->operator []("Instrument");
            newInfoObject.Instrument = NameOfArray;

            //get the time text
            QString StartTime = currentMap->operator []("Start");
            QString EndTime = currentMap->operator []("End");

            //convert the time text to DateTime objects
            newInfoObject.StartTime = textToDateTime(StartTime);
            newInfoObject.EndTime   = textToDateTime(EndTime);

            //Add the object to the DataSetInformation tracker
            this->DataSetInformation[NameOfArray].push_back(newInfoObject);
        }
    }
}

//==================================================================
void ScInfoPropWidget::buildDataSetGUIObjects()
{
    //need to cycle through the DataSet Information
    QStringList Keys = this->DataSetInformation.keys();

    //clear out the old
    ui->DataSet->clear();

    //build the new
    for(int x = 0; x < Keys.size(); x++)
    {

        //we need to make sure we don't activate unactivated keys
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(Keys[x].toAscii().data())) continue;

        //create the head object
        pqTreeWidgetItem *head = new pqTreeWidgetItem;
        QString headText = QString(Keys[x]);
        QString headText2 = this->InstrumentList[headText];
        head->setText(0,headText);
        head->setText(1, headText2);
        head->setTextColor(1, QColor("Dark Red"));


        //add childeren
        for(int y = 0; y < this->DataSetInformation[Keys[x]].size(); y ++)
        {
            //we must populate the array selectors
            QString Name = this->DataSetInformation[Keys[x]][y].Name;
            QString ID   = this->DataSetInformation[Keys[x]][y].ID;
            QString Inst = Keys[x];

            DateTime startDT = this->DataSetInformation[Keys[x]][y].StartTime;
            DateTime endDT   = this->DataSetInformation[Keys[x]][y].EndTime;

            //make the trackers for variables
            if(!this->VariablesSelectionTracker[Inst][ID])
            {
                this->VariablesSelectionTracker[Inst][ID] = vtkDataArraySelection::New();
            }

            //create the child item
            pqTreeWidgetItem *child = new pqTreeWidgetItem;
            child->setText(0, ID);
            child->setText(1, Name);
            if(this->DataSetSelectionTracker[Inst]->ArrayIsEnabled(ID.toAscii().data()))
            {
                child->setCheckState(0, Qt::Checked);
            }
            else
            {
                child->setCheckState(0, Qt::Unchecked);
            }
            child->setTextColor(0, QColor("Dark Blue"));

            //add the child to the head
            if(startDT <= this->startMJD && endDT >= this->endMJD)
            {
                QString tooltip = QString("Data for " + ID + " is available for dates " + QString::fromStdString(startDT.getDateTimeString()) + " to " + QString::fromStdString(endDT.getDateTimeString()) + "." );
                child->setToolTip(0, tooltip);
            }
            else
            {
                QString tooltip = QString("Data for " + ID + " is ONLY available for dates " + QString::fromStdString(startDT.getDateTimeString()) + " to " + QString::fromStdString(endDT.getDateTimeString()) + ". Data for the requested time is not available." );
                child->setToolTip(0, tooltip);
                child->setToolTip(1,tooltip);
                child->setTextColor(0, QColor("Gray"));
                child->setTextColor(1, QColor("Gray"));
            }
            head->addChild(child);

        }

        //add the item to the GUI
        ui->DataSet->addTopLevelItem(head);
    }

}


//==================================================================
void ScInfoPropWidget::extractVariableInfo()
{

    //Extracting  Information needed for gui
    QStringList Instruments = this->DataSetSelectionTracker.keys();

    //cycle through Instruments
    for(int x = 0; x < this->DataSetSelectionTracker.size(); x++)
    {
        //if the instrument isn't selected, skip it. This is a just-in-case call
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(Instruments[x].toAscii().data())) continue;

        //cycle through the datasets within the instrument
        for(int y =0; y < this->DataSetSelectionTracker[Instruments[x]]->GetNumberOfArrays();  y++)
        {

            //get the current data-set name
            QString currentDataSet = this->DataSetSelectionTracker[Instruments[x]]->GetArrayName(y);

            //check to see if the data-set is selected. if not skip.
            if(!this->DataSetSelectionTracker[Instruments[x]]->ArrayIsEnabled(currentDataSet.toAscii().data())) continue;

            //Now that we know we are dealing with the correct data sets, lets process it.
            filterNetworkList *item = this->DataSetVariableInfoCache[Instruments[x]][currentDataSet];

            if(!item)
            {
                std::cerr << "ERROR: Invalid Pointer" << std::endl;
                break;
            }

            //skip processing if we have already processed it.
            if(this->VariableSetInformation[Instruments[x]][currentDataSet].size() > 0) continue;

            //process the data
            for(int z = 0; z < item->size(); z++)
            {
                filterNetworkObject *currentMap = item->operator [](z);
                DataSetInfo newInfoObject;

                //extract the needed information and place in DataSetInfo Object
                newInfoObject.Name = currentMap->operator []("LongDescription");
                newInfoObject.ID   = currentMap->operator []("Name");

                //Add to the list
                this->VariableSetInformation[Instruments[x]][currentDataSet].push_back(newInfoObject);
            }
        }
    }
}

//==================================================================
void ScInfoPropWidget::buildVariableGUIObjects()
{

    //Build Variable GUI objects
    //get Instrument Names:
    QStringList Instrument = this->DataSetSelectionTracker.keys();

    //clear out old data
    ui->Variables->clear();

    //build new items
    for(int x = 0; x < this->DataSetSelectionTracker.size(); x++)
    {
        //skip if not activated
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(Instrument[x].toAscii().data())) continue;

        //set up Instrument Head
        pqTreeWidgetItem *IHead = new pqTreeWidgetItem;
        QString IHead_desc = this->InstrumentList[Instrument[x]];
        IHead->setText(0, Instrument[x]);
        IHead->setText(1, IHead_desc);
        IHead->setTextColor(1, QColor("Dark Green"));

        //State Information
        QString IHeadState = Instrument[x] + "::" + IHead_desc + ">>";

        //flag for displaying IHead
        int IHeadActiveFlag = 0;

        //process Data Sets
        for(int y = 0; y < this->DataSetSelectionTracker[Instrument[x]]->GetNumberOfArrays(); y++)
        {
            //get current Data Set
            QString currentDataSet = this->DataSetSelectionTracker[Instrument[x]]->GetArrayName(y);

            //skip if not activated
            if(!this->DataSetSelectionTracker[Instrument[x]]->ArrayIsEnabled(currentDataSet.toAscii().data())) continue;

            //create DataSet Head
            pqTreeWidgetItem *DSHead = new pqTreeWidgetItem;
            DataSetInfo currentDataInfo = this->DataSetInformation[Instrument[x]][y];
            DSHead->setText(0, currentDataSet);
            DSHead->setText(1, currentDataInfo.Name);
            DSHead->setTextColor(1, QColor("Dark Red"));

            //State Information
            QString HeadState = currentDataSet + "::" + currentDataInfo.Name + ">>";

            //Process the Varables List
            for(int z = 0; z < this->VariableSetInformation[Instrument[x]][currentDataSet].size(); z++)
            {
                //populate array selectors
                QString Name = this->VariableSetInformation[Instrument[x]][currentDataSet][z].Name;
                QString ID   = this->VariableSetInformation[Instrument[x]][currentDataSet][z].ID;

                //create the child item
                pqTreeWidgetItem *var = new pqTreeWidgetItem;
                var->setText(0, ID);
                var->setText(1, Name);

                //Save State
                QString VarState = IHeadState + HeadState + ID + "^^" + Name;

                //need a set flag to know if the base object should be displayed.
                if(this->VariablesSelectionTracker[Instrument[x]][currentDataSet]->ArrayIsEnabled(ID.toAscii().data()))
                {
                    var->setCheckState(0, Qt::Checked);
                }
                else
                {
                    var->setCheckState(0, Qt::Unchecked);
                }
                var->setTextColor(0, QColor("Dark Blue"));

                //add var to DataSet
                DSHead->addChild(var);

                //incriment IHead flag
                IHeadActiveFlag ++;

            }

            //add DSHead to Instrument
            IHead->addChild(DSHead);
        }

        //Add item to the GUI if active
        if(IHeadActiveFlag)
        {
            ui->Variables->addTopLevelItem(IHead);
        }
    }

}

//==================================================================
DateTime ScInfoPropWidget::textToDateTime(QString dateString)
{
    DateTime retVal;
    QStringList DateSplit;
    QStringList Date;
    QStringList Time;

    QStringList TimeSplit = dateString.split(".");

    if(TimeSplit.size() >= 1)
    {
        DateSplit = TimeSplit[0].split("T");
    }

    if(DateSplit.size() >= 2)
    {
        Date = DateSplit[0].split("-");
        Time = DateSplit[1].split(":");
    }
    if(Date.size() >= 3 && Time.size() >= 3)
    {
        retVal.setYear(Date[0].toInt());
        retVal.setMonth(Date[1].toInt());
        retVal.setDay(Date[2].toInt());

        retVal.setHours(Time[0].toInt());
        retVal.setMinutes(Time[1].toInt());
        retVal.setSeconds(Time[2].toInt());
    }

    return retVal;
}

//==================================================================
void ScInfoPropWidget::updateDataSet()
{
    this->getAllDataSetInfo();
    this->extractDataSetInfo();
    this->buildDataSetGUIObjects();
}

//==================================================================
void ScInfoPropWidget::instrumentSelectionChanged(QTreeWidgetItem* item, int status)
{
    //process the selection change
    if(this->InstrumentSelectionTracker->ArrayIsEnabled(item->text(0).toAscii().data()))
    {
        //we need to mark the item (and its childeren!) disabled.
        this->InstrumentSelectionTracker->DisableArray(item->text(0).toAscii().data());

        //turn off the selected Instruments Datasets
        this->DataSetSelectionTracker[item->text(0)]->DisableAllArrays();

    }
    else
    {
        this->InstrumentSelectionTracker->EnableArray(item->text(0).toAscii().data());
    }

#ifdef DEBUG
    std::cout << "Arrays in Instrument List:" << std::endl;
    for(int x = 0; x < this->InstrumentSelectionTracker->GetNumberOfArrays(); x++)
    {
        std::cout << "Name: " << this->InstrumentSelectionTracker->GetArrayName(x)
                  << " Status: " << ((this->InstrumentSelectionTracker->ArrayIsEnabled(this->InstrumentSelectionTracker->GetArrayName(x))) ? ("Enabled") : ("Disabled")) << std::endl;

    }

#endif

    //populate the next tree (Data Sets)
    updateDataSet();
    updateVariables();
}



//==================================================================
void ScInfoPropWidget::updateVariables()
{
    this->getAllVariableSetInfo();
    this->extractVariableInfo();
    this->buildVariableGUIObjects();
}


//==================================================================
void ScInfoPropWidget::cleanStateProperties()
{

    //clean dataset
    for(int x = 0; x < this->SaveStateDataSet->GetNumberOfElements(); x++)
    {
        this->SaveStateDataSet->SetElement(x, "");
    }
    this->SaveStateDataSet->SetNumberOfElements(1);

    //clean instruments
    for(int x = 0; x < this->SaveStateInstrument->GetNumberOfElements(); x++)
    {
        this->SaveStateInstrument->SetElement(x, "");
    }
    this->SaveStateInstrument->SetNumberOfElements(1);

    //clean Variables
    for(int x = 0; x < this->SaveStateVariables->GetNumberOfElements(); x++)
    {
        this->SaveStateVariables->SetElement(x, "");
    }
    this->SaveStateVariables->SetNumberOfElements(1);

}

//==================================================================
void ScInfoPropWidget::dataSetSelectionChanged(QTreeWidgetItem *item, int)
{
    QString Instrument = item->parent()->text(0);

    //process the selection change
    if(this->DataSetSelectionTracker[Instrument]->ArrayIsEnabled(item->text(0).toAscii().data()))
    {
        this->DataSetSelectionTracker[Instrument]->DisableArray(item->text(0).toAscii().data());

        //clear the selections for this item in Variables
        this->VariablesSelectionTracker[Instrument][QString(item->text(0))]->DisableAllArrays();
    }
    else
    {
        this->DataSetSelectionTracker[Instrument]->EnableArray(item->text(0).toAscii().data());
    }

#ifdef DEBUG
    std::cout << "Arrays in DataSet List:" << std::endl;
    for(int x = 0; x < this->DataSetSelectionTracker[Instrument]->GetNumberOfArrays(); x++)
    {
        std::cout << "Name: " << this->DataSetSelectionTracker[Instrument]->GetArrayName(x)
                  << " Instrument: " << Instrument.toStdString() << "status: " << ((this->DataSetSelectionTracker[Instrument]->ArrayIsEnabled(this->DataSetSelectionTracker[Instrument]->GetArrayName(x))) ? ("Enabled") : ("Disabled")) << std::endl;

    }
#endif

    //Populate the next tree (Variables)
    updateVariables();

}

//==================================================================
void ScInfoPropWidget::variableSelectionChanged(QTreeWidgetItem *item, int)
{
    QTreeWidgetItem *DSet = item->parent();
    QTreeWidgetItem *Inst = DSet->parent();

    QString DataSet = DSet->text(0);
    QString Instrument = Inst->text(0);

    //process the selection change
    if(this->VariablesSelectionTracker[Instrument][DataSet]->ArrayIsEnabled(item->text(0).toAscii().data()))
    {
        this->VariablesSelectionTracker[Instrument][DataSet]->DisableArray(item->text(0).toAscii().data());
    }
    else
    {
        this->VariablesSelectionTracker[Instrument][DataSet]->EnableArray(item->text(0).toAscii().data());
    }

#ifdef DEBUG
    std::cout << "Arrays in Variable List for [" << Instrument.toStdString() << "][" << DataSet.toStdString() << "]: " << std::endl;
    for(int x = 0; x < this->VariablesSelectionTracker[Instrument][DataSet]->GetNumberOfArrays(); x++)
    {
        std::cout << "Name: " << this->VariablesSelectionTracker[Instrument][DataSet]->GetArrayName(x)
                  << " Status: " << ((this->VariablesSelectionTracker[Instrument][DataSet]->ArrayIsEnabled(this->VariablesSelectionTracker[Instrument][DataSet]->GetArrayName(x))) ? ("Enabled") : ("Disabled")) << std::endl;

    }

#endif

}

//==================================================================
void ScInfoPropWidget::useAllVariables(bool state)
{
    if(!state)
    {
        ui->Variables->setVisible(true);
        ui->Variables_Label->setVisible(true);
        ui->allVariables_Label->setVisible(false);
        this->getAllVars = false;
    }
    else
    {
        ui->Variables->setVisible(false);
        ui->Variables_Label->setVisible(false);
        ui->allVariables_Label->setVisible(true);
        this->getAllVars = true;
    }
}


//==================================================================
void ScInfoPropWidget::timeRangeChanged()
{
    QDateTime end = ui->endTime->dateTime();
    QDateTime start = ui->startTime->dateTime();

    DateTime endDT;
    DateTime startDT;

    endDT.setYear(end.date().year());
    endDT.setMonth(end.date().month());
    endDT.setDay(end.date().day());

    endDT.setHours(end.time().hour());
    endDT.setMinutes(end.time().minute());
    endDT.setSeconds(end.time().second());

    startDT.setYear(start.date().year());
    startDT.setMonth(start.date().month());
    startDT.setDay(start.date().day());

    startDT.setHours(start.time().hour());
    startDT.setMinutes(start.time().minute());
    startDT.setSeconds(start.time().second());

    this->startMJD = startDT.getMJD();
    this->endMJD = endDT.getMJD();


}

//==================================================================
void ScInfoPropWidget::startTimeChanged()
{
    QDateTime end = ui->endTime->dateTime();
    QDateTime start = ui->startTime->dateTime();

    DateTime endDT;
    DateTime startDT;

    endDT.setYear(end.date().year());
    endDT.setMonth(end.date().month());
    endDT.setDay(end.date().day());

    endDT.setHours(end.time().hour());
    endDT.setMinutes(end.time().minute());
    endDT.setSeconds(end.time().second());

    startDT.setYear(start.date().year());
    startDT.setMonth(start.date().month());
    startDT.setDay(start.date().day());

    startDT.setHours(start.time().hour());
    startDT.setMinutes(start.time().minute());
    startDT.setSeconds(start.time().second());

    if(endDT < startDT)
    {
        ui->endTime->setDateTime(start);
        this->timeRangeChanged();

    }
}

//==================================================================
void ScInfoPropWidget::endTimeChanged()
{
    QDateTime end = ui->endTime->dateTime();
    QDateTime start = ui->startTime->dateTime();

    DateTime endDT;
    DateTime startDT;

    endDT.setYear(end.date().year());
    endDT.setMonth(end.date().month());
    endDT.setDay(end.date().day());

    endDT.setHours(end.time().hour());
    endDT.setMinutes(end.time().minute());
    endDT.setSeconds(end.time().second());

    startDT.setYear(start.date().year());
    startDT.setMonth(start.date().month());
    startDT.setDay(start.date().day());

    startDT.setHours(start.time().hour());
    startDT.setMinutes(start.time().minute());
    startDT.setSeconds(start.time().second());

    if(startDT > endDT)
    {
        ui->startTime->setDateTime(end);
        this->timeRangeChanged();
    }
}


//==================================================================
//get instrument list for current selections
bool ScInfoPropWidget::getInstrumentList()
{
    this->InstrumentList.clear();

    for(int x = 0; x < this->currentInstrumentObjects->size(); x++)
    {
        filterNetworkObject *currentMap = this->currentInstrumentObjects->operator [](x);

        QString name = currentMap->operator []("Name");
        QString desc = currentMap->operator []("LongDescription");

        this->InstrumentList.insert(name, desc);
    }

    if(!this->currentInstrumentObjects->isEmpty())
    {
        ObsGroupList.sort();
    }

    return true;
}


