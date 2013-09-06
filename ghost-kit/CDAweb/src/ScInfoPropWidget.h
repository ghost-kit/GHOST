#ifndef SCINFOPROPWIDGET_H
#define SCINFOPROPWIDGET_H

#include "pqPropertyWidget.h"
#include "pqOutputPort.h"
#include "filterNetworkAccessModule.h"
#include "DateTime.h"

#include <QWidget>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

#include <vtkSMProxy.h>
#include <vtkSMProperty.h>
#include <vtkSMStringVectorProperty.h>
#include <vtkDataArraySelection.h>


#include <QReadWriteLock>
#include <QTableWidgetItem>
#include "DateTime.h"

class DataSetInfo {
public:
    QString Name;
    QString LongName;
    QString ID;
    QString ObsGroup;
    QString Instrument;
    DateTime StartTime;
    DateTime EndTime;
};

namespace Ui {
class ScInfoPropWidget;
}

class ScInfoPropWidget : public pqPropertyWidget
{
    Q_OBJECT
    typedef pqPropertyWidget Superclass;

public:
    ScInfoPropWidget(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject = 0);
    ~ScInfoPropWidget();

    void apply();

protected:
    //Filter Components
    vtkSMProxy *smProxy;
    vtkSMProperty *smProperty;
    vtkSMStringVectorProperty *svp;

    //Time Settings
    double startMJD;
    double endMJD;

    //Data Selection Array Handlers

    //group selection handlers
    vtkDataArraySelection *GroupSelectionTracker;

    //observatory selection handlers
    vtkDataArraySelection *ObservatorySelectionTracker;

    //instrument selection handlers
    vtkDataArraySelection *InstrumentSelectionTracker;
    vtkDataArraySelection *InstrumentDataSetInfoCacheStatus;
    QMap<QString, filterNetworkList *> InstrumentDataSetInfoCache;

    //Data Set Selection handlers
    QMap<QString, QList<DataSetInfo> > DataSetInformation;
    QMap<QString, vtkDataArraySelection *> DataSetSelectionTracker;
    QMap<QString, vtkDataArraySelection *> DataSetVariableInfoCacheStatus;
    QMap<QString, QMap<QString, filterNetworkList *> > DataSetVariableInfoCache;

    //Variable Selection Trackers
    QMap<QString, vtkDataArraySelection *> VariablesSelectionTracker;







    //Cached Objects
    filterNetworkList *currentGroupObjects;
    filterNetworkList *currentInstrumentObjects;
    filterNetworkList *currentObservatoryObjects;

    //listings
    QStringList ObsGroupList;
    QStringList ObservatoryList;

    //maps
    QMap<QString , QString> InstrumentList;
    QStringList DataSetRetrunList;

    //current target
    QString currentGroup;
    QString currentObservatory;
    QString currentInstrument;
    QString currentDataSet;
    QMap <QString, QStringList> currentDataGroups;

    // URL Information
    QString baseURL;
    QString dataViewSpacePhys;
    QString getObservatorys;
    QString getObservatoryGroups;
    QString getInstrumentTypes;
    QString getInstruments;
    QString getInventory;
    QString getDataSets;
    QString getDataGroups;
    QString getVariables;

    //instrument Selection
    QList<QListWidgetItem*> selectedInstruments;

    //data selection
    QList<QListWidgetItem*> selectedData;
    QAtomicInt InstrumentLock;
    QAtomicInt DataLock;

    QAtomicInt ReprocessLock;

    QAtomicInt InstruemntSelectionsDenied;
    QAtomicInt DataSelectionDenied;

    QTableWidgetItem *dataColumn1;
    QTableWidgetItem *dataColumn2;


    //handlers
    bool getSCList(filterNetworkAccessModule &manager);
    bool getSCInstrument(filterNetworkAccessModule &manager);
    bool getSciDataGroup(filterNetworkAccessModule &manager, QString dataset);
    bool getSciVariables(filterNetworkAccessModule &manager, QString dataset);

    bool getGroupsList();
    bool getObservatoryList(QString Group);
    bool loadGroupListToGUI();

    bool getInstrumentList(double startTimes, double endTime);
    bool getDataSetsList();

    void getAllDataSetInfo();
    void getAllVariableSetInfo();

    void extractDataSetInfo();
    void buildDataSetGUIObjects();

    void extractVariableInfo();
    void buildVariableGUIObjects();

    DateTime textToDateTime(QString dateString);


private:
    Ui::ScInfoPropWidget *ui;

private slots:
    void selectedGroup(QString selection);
    void observatorySelectionChanged(QString selection);

    void instrumentSelectionChanged(QTreeWidgetItem*,int);
    void dataSetSelectionChanged(QTreeWidgetItem*item, int);

    void processDeniedInstrumentRequests();
    void processDeniedDataRequests();

    void timeRangeChanged();
    void startTimeChanged();
    void endTimeChanged();

signals:
    void recheckInstrumentSelections();
    void recheckDataSetSelction();
    void completedInstrumentProcessing();
    void completedDataProcessing();

};

#endif // SCINFOPROPWIDGET_H
