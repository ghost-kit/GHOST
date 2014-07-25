#ifndef GKPLOTSOURCEPANEL_H
#define GKPLOTSOURCEPANEL_H

#include "pqPropertyWidget.h"
#include "pqOutputPort.h"

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
#include <QListWidgetItem>
#include "qlist.h"
#include "QStringList"
#include "qstring.h"

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
#include <vtkSMIntVectorProperty.h>
#include <vtkDataArraySelection.h>
#include <vtkStringList.h>
#include <vtkNew.h>
#include <vtkCallbackCommand.h>

class gkPlotSourcePanel : public pqPropertyWidget
{
    Q_OBJECT
    typedef pqPropertyWidget Superclass;

public:
    gkPlotSourcePanel(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject = 0);
    ~gkPlotSourcePanel();

    virtual void apply();

protected:

    void updateGrid();

    //callbacks
    vtkCallbackCommand* infoObserver;

    static void infoCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

    vtkSMDoubleVectorProperty *gkGrid;
    vtkSMDoubleVectorProperty *gkData;
    vtkSMDoubleVectorProperty *gkExtents;

};

#endif // GKPLOTSOURCEPANEL_H
