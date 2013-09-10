//TODO:
//      2) Fix DataSet repeated querries
//      3) Fix Variable Selection
//      4) Optimize the call pattern for information

#include "ScInfoPropWidget.h"
#include "ui_ScInfoPropWidget.h"
#include "DateTime.h"
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

//==================================================================
ScInfoPropWidget::ScInfoPropWidget(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject)
    : Superclass(smproxy, parentObject),
      ui(new Ui::ScInfoPropWidget)
{

    this->smProperty = smproperty;
    this->smProxy = smproxy;

    this->svp = vtkSMStringVectorProperty::SafeDownCast(smproperty);

    if(!svp)
    {
        return;
    }

    //get properties
    vtkSMDoubleVectorProperty *timeStartProperties = vtkSMDoubleVectorProperty::SafeDownCast(smproxy->GetProperty("TimeRangeInfoStart"));
    this->startMJD = timeStartProperties->GetElement(0);

    vtkSMDoubleVectorProperty *timeEndProperties = vtkSMDoubleVectorProperty::SafeDownCast(smproxy->GetProperty("TimeRangeInfoEnd"));
    this->endMJD = timeEndProperties->GetElement(0);

    std::cout << "Start Time: " << std::setprecision(16)  << DateTime(this->startMJD).getDateTimeString() << std::endl;
    std::cout << "End Time:   " << DateTime(this->endMJD).getDateTimeString() << std::endl;


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

    //Load first set of Values
    filterNetworkAccessModule SCListManager;
    this->getSCList(SCListManager);
    this->currentGroupObjects = SCListManager.getFinalOjects();

    //get the observatory group list
    this->getGroupsList();

    //Load the group list
    this->loadGroupListToGUI();

    //Initialize trackers
    this->GroupSelectionTracker             = vtkDataArraySelection::New();
    this->ObservatorySelectionTracker       = vtkDataArraySelection::New();
    this->InstrumentSelectionTracker        = vtkDataArraySelection::New();
    this->InstrumentDataSetInfoCacheStatus  = vtkDataArraySelection::New();


    //all variable setup
    this->getAllVars = true;
    ui->allVariables->setChecked(true);
    ui->Variables->setVisible(false);
    ui->Variables_Label->setVisible(false);

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

    QStringList Instruments = this->DataSetSelectionTracker.keys();

    QString CodeString;

    for(int x = 0; x < this->InstrumentSelectionTracker->GetNumberOfArrays(); x ++)
    {
        //these are the instruments
        QString currentInstrument = Instruments[x];

        //if the current instrument is not enabled, skip
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(currentInstrument.toAscii().data())) continue;

        //build the instrument portion of the string
        if(x != 0)
        {
            CodeString = CodeString + ";";
        }

        //put in the current Instrument
        CodeString = CodeString + currentInstrument + ":";

        //Process the DataSets
        for(int y = 0; y < this->DataSetSelectionTracker[currentInstrument]->GetNumberOfArrays(); y++)
        {
            //these are the data sets
            QString currentDataSet = this->DataSetSelectionTracker[currentInstrument]->GetArrayName(y);

            //if the current data set is NOT enabled, skip
            if(!this->DataSetSelectionTracker[currentInstrument]->ArrayIsEnabled(currentDataSet.toAscii().data())) continue;

            //Add the current Data Set
            if(y != 0)
            {
                CodeString = CodeString + ",";
            }

            CodeString = CodeString + currentDataSet + "~";

            for(int z = 0; z < this->VariablesSelectionTracker[currentInstrument][currentDataSet]->GetNumberOfArrays(); z++)
            {
                //these are the actual variables
                QString VarName = this->VariablesSelectionTracker[currentInstrument][currentDataSet]->GetArrayName(z);

                //if var not active, skip
                if(!this->VariablesSelectionTracker[currentInstrument][currentDataSet]->ArrayIsEnabled(VarName.toAscii().data())) continue;

                //add variables
                if(z != 0)
                {
                    CodeString = CodeString + "|";
                }

                CodeString = CodeString + VarName;

            }
        }
    }

    std::cerr << "Code String: " << CodeString.toStdString() << std::endl;

    //Add relevent information to ParaView Property
    this->svp->SetElement(0, this->currentGroup.toAscii().data());
    this->svp->SetElement(1, this->currentObservatory.toAscii().data());
    this->svp->SetElement(2, CodeString.toAscii().data());

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

        std::cout << "Set Start DateTime to: " << startDT.getDateTimeString() << std::endl;
        std::cout << "Set End DateTime to: " << endDT.getDateTimeString() << std::endl;


        vtkSMDoubleVectorProperty *timeRange =  vtkSMDoubleVectorProperty::SafeDownCast(this->smProxy->GetProperty("TimeRange"));
        timeRange->SetElement(0,startDT.getMJD());
        timeRange->SetElement(1,endDT.getMJD());
    }

    //apply the upstream parameters
    Superclass::apply();

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

    std::cout << QString(this->baseURL + this->dataViewSpacePhys + this->getInstrumentTypes
                         + "?observatory=" + this->currentObservatory).toAscii().data() << std::endl;

    manager.Get(this->baseURL + this->dataViewSpacePhys + this->getInstruments
                + "?observatory=" + this->currentObservatory,
                QString("Instruments"),
                QString("InstrumentDescription"));
    return true;

}

//==================================================================
bool ScInfoPropWidget::getSciDataGroup(filterNetworkAccessModule &manager, QString dataset)
{
    std::cout << QString(this->baseURL + this->dataViewSpacePhys + this->getDataGroups).toAscii().data() << std::endl;

    manager.Get(this->baseURL + this->dataViewSpacePhys + this->getDataSets + "?observatoryGroup="
                + this->currentGroup + "&observatory=" + this->currentObservatory + "&instrument=" + dataset ,
                QString("Datasets"),
                QString("DatasetDescription"));

    return true;

}

//==================================================================
bool ScInfoPropWidget::getSciVariables(filterNetworkAccessModule &manager, QString dataset)
{
    std::cout << QString(this->baseURL + this->dataViewSpacePhys + this->getDataGroups).toAscii().data() << std::endl;

    manager.Get(this->baseURL + this->dataViewSpacePhys + this->getDataSets + "/" + dataset + "/" + this->getVariables,
                QString("Variables"),
                QString("VariableDescription"));
    return true;
}

//==================================================================
//parse for groups
bool ScInfoPropWidget::getGroupsList()
{

    std::cout << "Size of List: " << this->currentGroupObjects->size() << std::endl;

    for(int x = 0; x < this->currentGroupObjects->size(); x++)
    {
        filterNetworkObject *currentMap = this->currentGroupObjects->operator [](x);

        QList<QString> keys = currentMap->keys();

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
void ScInfoPropWidget::groupSelectedChanged(QString selection)
{
    std::cout << "Group Selected: " << selection.toAscii().data() << std::endl;

    this->currentGroup = selection;
    this->currentObservatory = "";

    if(selection != "--- Select Group ---")
    {
        this->getObservatoryList(selection);
        ui->Observatory->clear();
        ui->Observatory->addItem("--- Select Observatory ---");
        ui->Observatory->addItems(this->ObservatoryList);
    }
    else
    {
        ui->Observatory->clear();
    }

    ui->Instruments->clear();
    ui->Variables->clear();
    ui->DataSet->clear();

}

//==================================================================
//process Observatories
void ScInfoPropWidget::observatorySelectionChanged(QString selection)
{

    //clear the Instrument Tracker
    this->InstrumentSelectionTracker->RemoveAllArrays();

    //clear DataSet and Variables TODO: Check to see if we need to delete the vtkDataSetSelection objects first.
    this->DataSetSelectionTracker.clear();
    this->VariablesSelectionTracker.clear();

    //set the observatory selection information
    this->currentObservatory = selection;

    //create network access module
    filterNetworkAccessModule SCInstrumentManager;

    //get data for next item
    this->getSCInstrument(SCInstrumentManager);
    this->currentInstrumentObjects = SCInstrumentManager.getFinalOjects();

    //get the required list
    this->getInstrumentList(this->startMJD, this->endMJD);

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

            /**** THE BELOW NEEDS TO BE MOVED TO A NEW METHOD FOR POPULATING TREE LISTS ****/
            /**** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv ****/

            //populate the GUI tree items
            pqTreeWidgetItem *newItem = new pqTreeWidgetItem();

            newItem->setText(0,InstrumentNames[x] );
            newItem->setText(1,InstrumentDesc[x] );
            newItem->setTextColor(0,QColor("Dark Blue"));
            newItem->setCheckState(0,Qt::Unchecked);

            //place in the GUI
            ui->Instruments->addTopLevelItem(newItem);

            /**** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ****/
            /**** END NEED TO BE MOVED ****/

        }
    }

}


//==================================================================
//Instrument Selection has Changed
void ScInfoPropWidget::getAllDataSetInfo()
{

    for(int x = 0; x < this->InstrumentSelectionTracker->GetNumberOfArrays(); x ++)
    {
        //get name of current array
        QString NameOfArray = QString(this->InstrumentSelectionTracker->GetArrayName(x));

        //check to see if the data is selected
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(NameOfArray.toAscii().data())) continue;

        std::cout << "Processing " << NameOfArray.toStdString() << std::endl;

        //if the data is already in the cache, we don't need to download it
        if(this->InstrumentDataSetInfoCacheStatus->ArrayIsEnabled(NameOfArray.toAscii().data())) continue;

        std::cout << "Downloading Data for " << NameOfArray.toStdString() << std::endl;

        //Download the data
        filterNetworkAccessModule SCDataSetListManager;
        this->getSciDataGroup(SCDataSetListManager, NameOfArray);

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
            std::cout << "Processing Instrument " << Instruments[x].toStdString() << std::endl;
            //get keys of data sets
            int numberArrays = this->DataSetSelectionTracker[Instruments[x]]->GetNumberOfArrays();

            for(int y =0; y < numberArrays; y++)
            {

                //get information on the Variables in DataSets
                QString InstrumentName = Instruments[x];
                QString DataSetName    = DataSetSelectionTracker[Instruments[x]]->GetArrayName(y);

                //check to see if the data is selected
                if(!this->DataSetSelectionTracker[Instruments[x]]->ArrayIsEnabled(DataSetName.toAscii().data())) continue;

                std::cout << "Processing DataSet " << DataSetName.toStdString() << std::endl;

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

    std::cout << "Processing the Data Set List for GUI" << std::endl;

    for(int x = 0; x < this->InstrumentSelectionTracker->GetNumberOfArrays(); x ++)
    {
        QString NameOfArray = QString(InstrumentSelectionTracker->GetArrayName(x));

        //skip if not enabled
        if(!this->InstrumentSelectionTracker->ArrayIsEnabled(NameOfArray.toAscii().data())) continue;

        //process the enabled arrays
        std::cout << "Processing GUI for Instrument: " << NameOfArray.toStdString() << std::endl;

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
    std::cout << "Building DataSet GUI Objects" << std::endl;

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

            std::cout << "Name: " << Name.toStdString() << std::endl;
            std::cout << "ID:   " << ID.toStdString() << std::endl;
            std::cout << "Instrument: " << Inst.toStdString() << std::endl;

            //make the trackers for variables
            this->VariablesSelectionTracker[Inst][ID] = vtkDataArraySelection::New();


            //create the child item
            pqTreeWidgetItem *child = new pqTreeWidgetItem;
            child->setText(0, ID);
            child->setText(1, Name);
            if(this->DataSetSelectionTracker[Inst]->ArrayIsEnabled(ID.toAscii().data()))
            {
                std::cout << "Value is active" << std::endl;
                child->setCheckState(0, Qt::Checked);
            }
            else
            {
                std::cout << "Value is inactive" << std::endl;
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

            std::cout << "==="    << std::endl;

        }

        //add the item to the GUI
        ui->DataSet->addTopLevelItem(head);
    }

}


//==================================================================
void ScInfoPropWidget::extractVariableInfo()
{

    //Extracting  Information needed for gui
    std::cout << "Processing Variable List" << std::endl;

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

                std::cout << "Adding Variable: " << newInfoObject.Name.toStdString() << " for DataSet: " << currentDataSet.toStdString() << " for Instrument: " << Instruments[x].toStdString() << std::endl;
            }
        }
    }
}

//==================================================================
void ScInfoPropWidget::buildVariableGUIObjects()
{
    //Build Variable GUI objects
    std::cout << "Building Variable GUI Objects" << std::endl;

    //get Instrument Names:
    QStringList Instrument = this->DataSetSelectionTracker.keys();

    //clear out old data
    ui->Variables->clear();

    std::cout << "Variables GUI Cleared" << std::endl << std::flush;

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

        //flag for displaying IHead
        int IHeadActiveFlag = 0;

        //process Data Sets
        for(int y = 0; y < this->DataSetSelectionTracker[Instrument[x]]->GetNumberOfArrays(); y++)
        {
            //get current Data Set
            QString currentDataSet = this->DataSetSelectionTracker[Instrument[x]]->GetArrayName(y);
            std::cout << "Got Array Name" << std::endl << std::flush;

            //skip if not activated
            if(!this->DataSetSelectionTracker[Instrument[x]]->ArrayIsEnabled(currentDataSet.toAscii().data())) continue;

            //create DataSet Head
            pqTreeWidgetItem *DSHead = new pqTreeWidgetItem;
            DataSetInfo currentDataInfo = this->DataSetInformation[Instrument[x]][y];
            DSHead->setText(0, currentDataSet);
            DSHead->setText(1, currentDataInfo.Name);
            DSHead->setTextColor(1, QColor("Dark Red"));


            //Process the Varables List
            for(int z = 0; z < this->VariableSetInformation[Instrument[x]][currentDataSet].size(); z++)
            {
                std::cout << "\nProcessing index " << z << " of " << this->VariableSetInformation[Instrument[x]][currentDataSet].size() - 1 << std::endl << std::flush;
                //populate array selectors
                QString Name = this->VariableSetInformation[Instrument[x]][currentDataSet][z].Name;
                QString ID   = this->VariableSetInformation[Instrument[x]][currentDataSet][z].ID;

                std::cout << "Variable:   " << Name.toStdString() << std::endl;
                std::cout << "ID:         " << ID.toStdString() << std::endl;
                std::cout << "Instrument: " << Instrument[x].toStdString() << std::endl;
                std::cout << "DataSet:    " << currentDataSet.toStdString() << std::endl;

                //create the child item
                pqTreeWidgetItem *var = new pqTreeWidgetItem;
                var->setText(0, ID);
                var->setText(1, Name);

                std::cout << "Created Variable Object for " << Name.toStdString() << std::endl << std::flush;

                //need a set flag to know if the base object should be displayed.


                if(this->VariablesSelectionTracker[Instrument[x]][currentDataSet]->ArrayIsEnabled(ID.toAscii().data()))
                {
                    std::cout << "Object is Enabled" << std::endl << std::flush;
                    var->setCheckState(0, Qt::Checked);
                }
                else
                {
                    std::cout << "Object is Disabled" << std::endl << std::flush;
                    var->setCheckState(0, Qt::Unchecked);
                }
                var->setTextColor(0, QColor("Dark Blue"));

                std::cout << "Color set to Blue" << std::endl << std::flush;

                //add var to DataSet
                DSHead->addChild(var);
                std::cout << "Added to DataSet Head" << std::endl << std::flush;

                //incriment IHead flag
                IHeadActiveFlag ++;

            }

            //add DSHead to Instrument
            IHead->addChild(DSHead);
            std::cout << "Added to Instrument Head" << std::endl << std::flush;
        }

        //Add item to the GUI if active
        if(IHeadActiveFlag)
        {
            ui->Variables->addTopLevelItem(IHead);
            std::cout << "Added to GUI" << std::endl << std::flush;
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
    std::cout << "Converted From Text to DT: " << retVal.getDateTimeString() << std::endl;

    return retVal;
}

//==================================================================
void ScInfoPropWidget::instrumentSelectionChanged(QTreeWidgetItem* item, int status)
{

    //process the selection change
    if(this->InstrumentSelectionTracker->ArrayIsEnabled(item->text(0).toAscii().data()))
    {
        //we need to mark the item (and its childeren!) disabled.
        this->InstrumentSelectionTracker->DisableArray(item->text(0).toAscii().data());
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
    this->getAllDataSetInfo();
    this->extractDataSetInfo();
    this->buildDataSetGUIObjects();


}



//==================================================================
void ScInfoPropWidget::dataSetSelectionChanged(QTreeWidgetItem *item, int)
{

    std::cout << "Data Set Selection Has Changed" << std::endl;

    QString Instrument = item->parent()->text(0);

    std::cout << "Instrument: " << Instrument.toStdString() << std::endl;
    std::cout << "Item:       " << item->text(0).toStdString() << std::endl;

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
    this->getAllVariableSetInfo();
    this->extractVariableInfo();
    this->buildVariableGUIObjects();

}

//==================================================================
void ScInfoPropWidget::variableSelectionChanged(QTreeWidgetItem *item, int)
{

    std::cout << "Variable Selection Has Changed" << std::endl;

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
        this->getAllVars = true;
    }
    else
    {
        ui->Variables->setVisible(false);
        ui->Variables_Label->setVisible(false);
        ui->allVariables_Label->setVisible(true);
        this->getAllVars = false;
    }
}


//==================================================================
void ScInfoPropWidget::timeRangeChanged()
{


    std::cout << "Time Range has been changed" << std::endl;
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
    std::cout << "Start Time Changed" << std::endl;
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
        std::cout << "fixing end time" << std::endl;
        ui->endTime->setDateTime(start);
        this->timeRangeChanged();

    }
}

//==================================================================
void ScInfoPropWidget::endTimeChanged()
{
    std::cout << "End Time Changed" << std::endl;
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
        std::cout << "fixing start time" << std::endl;
        ui->startTime->setDateTime(end);
        this->timeRangeChanged();
    }
}


//==================================================================
//get instrument list for current selections
bool ScInfoPropWidget::getInstrumentList(double startTimes, double endTime)
{

    std::cout << "Getting the Data List" << std::endl;
    this->InstrumentList.clear();

    for(int x = 0; x < this->currentInstrumentObjects->size(); x++)
    {
        filterNetworkObject *currentMap = this->currentInstrumentObjects->operator [](x);

        QString name = currentMap->operator []("Name");
        QString desc = currentMap->operator []("LongDescription");

        std::cout << "Name: " << name.toAscii().data() << " Description: " << desc.toAscii().data() << std::endl;

        this->InstrumentList.insert(name, desc);

    }

    if(!this->currentInstrumentObjects->isEmpty())
    {
        ObsGroupList.sort();
    }

    return true;
}


