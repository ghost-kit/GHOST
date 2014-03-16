#ifndef GK_SELECTIONBOXWIDGET_H
#define GK_SELECTIONBOXWIDGET_H

#include <QWidget>
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

namespace Ui {
class gk_selectionBoxWidget;
}

class gk_selectionBoxWidget : public pqPropertyWidget
{
    Q_OBJECT
    typedef pqPropertyWidget Superclass;

public:

    explicit gk_selectionBoxWidget(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject = 0);
    ~gk_selectionBoxWidget();

    virtual void apply();
    virtual void reset();

protected:
    vtkSMStringVectorProperty* inInfoProperty;
    vtkSMStringVectorProperty* currentProperty;

    void processProperty();

    //callbacks
    vtkCallbackCommand* infoObserver;

    static void infoCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);

private:
    Ui::gk_selectionBoxWidget *ui;
};

#endif // GK_SELECTIONBOXWIDGET_H
