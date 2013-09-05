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


    //Setup the UI
    this->dataColumn1 = new QTableWidgetItem("Instrument");
    this->dataColumn2 = new QTableWidgetItem("Description");

    ui->setupUi(this);
    ui->gridLayout->setMargin(pqPropertiesPanel::suggestedMargin());
    ui->gridLayout->setHorizontalSpacing(pqPropertiesPanel::suggestedHorizontalSpacing());
    ui->gridLayout->setVerticalSpacing(pqPropertiesPanel::suggestedVerticalSpacing());

    ui->Observatory->setDisabled(true);
    ui->Instruments->setDisabled(true);
    // create tree widget selection helper
//    new pqTreeWidgetSelectionHelper(ui->Instruments);

    ui->DataSet->setDisabled(true);
    // create tree widget selection helper
//    new pqTreeWidgetSelectionHelper(ui->DataSet);

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



    //connect signals to slots

    /** Time Connections */
    connect(ui->startTime, SIGNAL(editingFinished()), this, SLOT(timeRangeChanged()));
    connect(ui->startTime, SIGNAL(editingFinished()), this, SLOT(startTimeChanged()));
    connect(ui->endTime, SIGNAL(editingFinished()), this, SLOT(timeRangeChanged()));
    connect(ui->endTime, SIGNAL(editingFinished()), this, SLOT(endTimeChanged()));

    /** Group Connections */
    connect(ui->Group, SIGNAL(activated(QString)), this, SLOT(selectedGroup(QString)));

    /** Observatory Connections */
    connect(ui->Observatory, SIGNAL(activated(QString)), this, SLOT(observatorySelectionChanged(QString)));

    /** Instrument Connections */
    this->connect(ui->Instruments, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(instrumentSelectionChanged(QTreeWidgetItem*,int)));

    /** Data Selection Changed */
    connect(ui->DataSet, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(dataSetSelectionChanged(QTreeWidgetItem*,int)));

    /** Property Links to trigger Apply button active */
    this->addPropertyLink(ui->Variables, smproxy->GetPropertyName(smproperty), SIGNAL(itemSelectionChanged()), this->svp);
    this->addPropertyLink(ui->startTime, smproxy->GetPropertyName(smproperty), SIGNAL(editingFinished()), this->svp);
    this->addPropertyLink(ui->endTime, smproxy->GetPropertyName(smproperty), SIGNAL(editingFinished()), this->svp);

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

    //build a list of elements
    QList<QTreeWidgetItem *> selectedElements = ui->DataSet->selectedItems();
    QList<QTreeWidgetItem *>::Iterator iter;

    QMap<QString, QStringList> DataMap;

    //Get Instruments and Keys
    for(iter = selectedElements.begin(); iter != selectedElements.end(); ++iter)
    {
        QString Instrument;

        //Make sure we only get the Elements with Parents (i.e. non-insturment slecetions)
        if((*iter)->parent())
        {
            Instrument = (*iter)->parent()->text(0);
            DataMap[Instrument].push_back((*iter)->text(1));
        }
    }



    //get the list of variables
    QList<QTreeWidgetItem *> selectedVariables = ui->Variables->selectedItems();

    QMap<QString,QStringList> VariableMap;

    for(iter = selectedVariables.begin(); iter != selectedVariables.end(); ++iter)
    {

        QMap<QString, QString> DataSet;
        QString Data;

        if((*iter)->parent())
        {
            Data = (*iter)->parent()->text(0);
            Data = this->DataList[(*iter)->parent()->text(1)].key(Data);
            VariableMap[Data].push_back(this->VariableList[(*iter)->parent()->text(0)].key((*iter)->text(0)));

            //            std::cout << "DataSet: " << Data.toAscii().data() << std::endl;
            //            std::cout << "Variable: " << VariableMap[Data].back().toAscii().data() << std::endl;
            //            std::cout << "==========" << std::endl;
        }
    }


    //Create the needed string
    //  Insturments separated by ;
    //  Data sets separated by ,
    QString DataString;

    QStringList keys = DataMap.keys();
    QList<QStringList> values = DataMap.values();

    for(int x = 0; x < keys.size(); x++)
    {
        if(x != 0)
        {
            DataString = DataString + ";";
        }

        DataString = DataString + keys[x] + ":";

        for(int y = 0; y < values[x].size(); y++)
        {
            if(y != 0)
            {
                DataString = DataString + "," ;
            }

            DataString = DataString + values[x][y] + "~" ;

            for(int g = 0; g < VariableMap[values[x][y]].size(); g++)
            {
                if(g != 0)
                {
                    DataString = DataString + "|";
                }

                DataString = DataString + VariableMap[values[x][y]][g];
            }
        }

    }

    std::cerr << "CodeString: " << DataString.toAscii().data() << std::endl;

    this->svp->SetElement(0, this->currentGroup.toAscii().data());
    this->svp->SetElement(1, this->currentObservatory.toAscii().data());
    this->svp->SetElement(2, DataString.toAscii().data());

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
bool ScInfoPropWidget::getDataSetsList()
{

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

    ui->Observatory->setDisabled(true);
    ui->Instruments->setDisabled(true);
    ui->DataSet->setDisabled(true);
    ui->Variables->setDisabled(true);

    return true;
}

//==================================================================
//process the change in selection on the selection box
void ScInfoPropWidget::selectedGroup(QString selection)
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
        ui->Observatory->setEnabled(true);
    }
    else
    {
        ui->Observatory->clear();
        ui->Observatory->setDisabled(true);
    }

    //clear downstream elements
    this->currentInstrument = "";



    ui->Instruments->setDisabled(true);
    ui->DataSet->setDisabled(true);

    this->currentDataSet = "";
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
    ui->Variables->setDisabled(true);
    ui->DataSet->setDisabled(true);
    ui->Instruments->clear();
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

    //set the remaining tree lists...  TODO: this will need to be rethought a bit.
    ui->Instruments->setEnabled(true);
    ui->DataSet->setDisabled(true);
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
        this->InstrumentDataSetInfoCache[NameOfArray] = SCDataSetListManager.getFinalOjects();
        this->InstrumentDataSetInfoCacheStatus->EnableArray(NameOfArray.toAscii().data());

    }


}

//==================================================================
void ScInfoPropWidget::getAllVariableSetInfo()
{
//    int count = 0;

//    QMap<QString, QList<filterNetworkList *> > newVariableObjectGroup;

//    QMap<QString, QStringList>::Iterator iter;
//    for(iter = DataSetList.begin(); iter != DataSetList.end(); ++iter)
//    {
//        QStringList Instrument = DataSetList.keys();
//        QList<QStringList> DataSets = DataSetList.values();

//        for(int i = 0; i < DataSets[count].size(); i++)
//        {
//            //Configure the Needs for Variables Selection List
//            filterNetworkAccessModule SCVariableListManager;
//            this->getSciVariables(SCVariableListManager, DataSets[count][i]);

//            QString DataName = this->DataList[Instrument[count]][DataSets[count][i]];

//            newVariableObjectGroup[Instrument[count] + "\t" + DataName].append(SCVariableListManager.getFinalOjects());
//        }
//        count ++;
//    }
//    //this is the poor mans way of defeating the race condition...
//    //still need to fix out of order access when copying...
//    this->currentVariablesObjects = newVariableObjectGroup;
}

//==================================================================
void ScInfoPropWidget::extractDataSetInfo()
{
    //This method extracts needed information from the XML

    std::cout << "Processing the List for GUI" << std::endl;

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
        head->setText(0,headText);

        //add childeren
        for(int y = 0; y < this->DataSetInformation[Keys[x]].size(); y ++)
        {
            //we must populate the array selectors
            QString Name = this->DataSetInformation[Keys[x]][y].Name;
            QString ID   = this->DataSetInformation[Keys[x]][y].ID;
            QString Inst = Keys[x];

            std::cout << "Name: " << Name.toStdString() << std::endl;
            std::cout << "ID:   " << ID.toStdString() << std::endl;
            std::cout << "Instrument: " << Inst.toStdString() << std::endl;
            std::cout << "==="    << std::endl;

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
            head->addChild(child);

        }

        //add the item to the GUI
        ui->DataSet->addTopLevelItem(head);
    }

    //enable the ui
    ui->DataSet->setDisabled(false);

}


//==================================================================
void ScInfoPropWidget::extractVariableInfo()
{
    QMap<QString, QList<filterNetworkList *> >::Iterator iter;
    QList<filterNetworkList *>::Iterator iter2;

    int count = 0;

    //    ui->Variables->clear();
    this->VariableList.clear();

    QMap<QString, QMap<QString, QString> > List;

    for(iter=this->currentVariablesObjects.begin(); iter != this->currentVariablesObjects.end(); ++iter)
    {
        QStringList keys = this->currentVariablesObjects.keys();

        QList<filterNetworkList *> item = (*iter);

        QList<QTreeWidgetItem*> treelist;
        QMap<QString, QString> temp;

        QStringList DataSet = keys[count].split("\t");


        for(iter2= item.begin(); iter2 != item.end(); ++iter2)
        {
            filterNetworkList *item2 = (*iter2);

            for(int x = 0; x < item2->size(); x++)
            {
                filterNetworkObject *currentMap = item2->operator [](x);

                QString Name = currentMap->operator []("Name");
                QString Desc = currentMap->operator []("LongDescription");

                temp.insert(Name, Desc);

                QTreeWidgetItem * child = new QTreeWidgetItem();
                child->setText(0, Desc);
                child->setText(1, Name);

                treelist.push_back(child);

            }
            List.insert(DataSet[1], temp);
        }

        count ++;

        QTreeWidgetItem *newItem = new QTreeWidgetItem();
        newItem->setText(0,DataSet[1]);     //this is the DataSet
        newItem->setText(1, DataSet[0]);    //this is the Instrument
        newItem->setTextColor(0, QColor("dark blue"));
        newItem->addChildren(treelist);

        ui->Variables->setColumnCount(2);
        ui->Variables->hideColumn(1);
        ui->Variables->addTopLevelItem(newItem);
        ui->Variables->setEnabled(true);
        ui->Variables->expandAll();

    }

    this->VariableList = List;
}

//==================================================================
void ScInfoPropWidget::buildVariableGUIObjects()
{
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

//    if(this->DataLock.testAndSetAcquire(0,1))
//    {

//        QList<QTreeWidgetItem*> dataSets = ui->DataSet->selectedItems();

//        QMap<QString, QStringList> DataMap;

//        if(!dataSets.isEmpty())
//        {
//            //create a list of items
//            QList<QTreeWidgetItem*>::iterator iter;
//            for(iter = dataSets.begin(); iter != dataSets.end(); ++iter)
//            {
//                QString Instrument;

//                //Make sure we only get the Elements with Parents (i.e. non-insturment slecetions)
//                if((*iter)->parent())
//                {
//                    Instrument = (*iter)->parent()->text(0);
//                    DataMap[Instrument].push_back((*iter)->text(1));
//                }
//            }
//            getAllVariableSetInfo(DataMap);
//        }

//        this->setupVariableSets();
//        this->DataLock.deref();

//        connect(this, SIGNAL(completedDataProcessing()), this, SLOT(processDeniedDataRequests()));

//        if(this->DataSelectionDenied.testAndSetAcquire(1,1))
//        {
//            emit this->completedDataProcessing();
//        }
//    }
//    else
//    {
//        this->DataSelectionDenied.testAndSetAcquire(0,1);
//    }



//==================================================================
void ScInfoPropWidget::processDeniedInstrumentRequests()
{

    if(this->InstruemntSelectionsDenied.testAndSetAcquire(1,0))
    {
        emit this->recheckInstrumentSelections();
    }

}

//==================================================================
void ScInfoPropWidget::processDeniedDataRequests()
{


    if(this->DataSelectionDenied.testAndSetAcquire(1,0))
    {
        std::cout << "Retrying requests" << std::endl;

        emit this->recheckDataSetSelction();
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


