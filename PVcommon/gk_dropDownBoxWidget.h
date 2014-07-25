#ifndef GK_DROPDOWNBOXWIDGET_H
#define GK_DROPDOWNBOXWIDGET_H

//LOCAL
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
class gk_dropDownBoxWidget;
}

class gk_dropDownBoxWidget : public pqPropertyWidget
{
    Q_OBJECT
    typedef pqPropertyWidget Superclass;

public:
     gk_dropDownBoxWidget(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject = 0);
    ~gk_dropDownBoxWidget();

    virtual void apply();
    virtual void reset();

protected:

    vtkSMStringVectorProperty *availableFieldsNames;
    vtkSMStringVectorProperty *selectedFieldName;
    vtkSMIntVectorProperty    *selectedField;

    //Information Property
    vtkSMStringVectorProperty *infoProperty;
    vtkSMStringVectorProperty *outPorperty;

    vtkSMProxy  *smProxy;

    //helpers
    void updateDropDownList();

    //callbacks
    vtkCallbackCommand* infoObserver;

    static void infoCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);


private slots:
    void selectionChanged(QString selection);
    void onChangeFinished();
private:
    Ui::gk_dropDownBoxWidget *ui;
};

#endif // GK_DROPDOWNBOXWIDGET_H
