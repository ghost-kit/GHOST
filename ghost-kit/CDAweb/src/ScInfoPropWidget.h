#ifndef SCINFOPROPWIDGET_H
#define SCINFOPROPWIDGET_H

//LOCAL
#include "pqPropertyWidget.h"
#include "pqOutputPort.h"
#include "filterNetworkAccessModule.h"
#include "ltrDateTime.h"

//QT
#include <QWidget>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QString>
#include <QReadWriteLock>
#include <QTableWidgetItem>
#include <QComboBox>

//VTK
#include <vtkSMProxy.h>
#include <vtkSMProperty.h>
#include <vtkSMStringVectorProperty.h>
#include <vtkSMDoubleVectorProperty.h>
#include <vtkDataArraySelection.h>
#include <vtkStringList.h>
#include <vtkNew.h>

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
    void reset();

    void updateDataSet();
    void updateVariables();
    void setInstumentGUI();
protected:
    //Filter Components
    vtkSMProxy *smProxy;
    vtkSMProperty *smProperty;
    vtkSMStringVectorProperty *svp;

    //Time Settings
    double startMJD;
    double endMJD;

    //state helpers
    void cleanStateProperties();


    //**Data Selection Array Handlers**//
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
    QMap<QString, QMap<QString, QList<DataSetInfo> > > VariableSetInformation;
    QMap<QString, QMap<QString, vtkDataArraySelection *> > VariablesSelectionTracker;
    bool getAllVars;


    //Cached Objects
    filterNetworkList *currentGroupObjects;
    filterNetworkList *currentInstrumentObjects;
    filterNetworkList *currentObservatoryObjects;

    //listings
    QStringList ObsGroupList;
    QStringList ObservatoryList;

    //maps
    QMap<QString , QString> InstrumentList;

    //current target
    QString currentGroup;
    QString currentObservatory;


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


    //handlers
    void setGroupGUI();
    void restoreGUIinit();

    bool getSCList(filterNetworkAccessModule &manager);
    bool getSCInstrument(filterNetworkAccessModule &manager);
    bool getSciDataGroup(filterNetworkAccessModule &manager, QString dataset);
    bool getSciVariables(filterNetworkAccessModule &manager, QString dataset);

    bool getGroupsList();
    bool getObservatoryList(QString Group);
    void setObservatoryGUI();

    bool loadGroupListToGUI();

    bool getInstrumentList();

    void getAllDataSetInfo();
    void getAllVariableSetInfo();

    void extractDataSetInfo();
    void buildDataSetGUIObjects();

    void extractVariableInfo();
    void buildVariableGUIObjects();

    DateTime textToDateTime(QString dateString);

    //Active Selection State
    vtkSMStringVectorProperty *SaveStateGroup;
    vtkSMStringVectorProperty *SaveStateObservatory;
    vtkSMStringVectorProperty *SaveStateInstrument;
    vtkSMStringVectorProperty *SaveStateDataSet;
    vtkSMStringVectorProperty *SaveStateVariables;

    //Time State
    vtkSMDoubleVectorProperty *startTimeState;
    vtkSMDoubleVectorProperty *endTimeState;

private:
    Ui::ScInfoPropWidget *ui;

private slots:
    void groupSelectedChanged(QString selection);
    void observatorySelectionChanged(QString selection);

    void instrumentSelectionChanged(QTreeWidgetItem*,int);
    void dataSetSelectionChanged(QTreeWidgetItem*item, int);
    void variableSelectionChanged(QTreeWidgetItem*item, int);

    void useAllVariables(bool state);

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
