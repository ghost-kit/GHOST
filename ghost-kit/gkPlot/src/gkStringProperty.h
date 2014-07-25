#ifndef GKSTRINGPROPERTY_H
#define GKSTRINGPROPERTY_H

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
class gkStringProperty;
}

class gkStringProperty : public pqPropertyWidget
{
    Q_OBJECT
    typedef pqPropertyWidget Superclass;

public:
    explicit gkStringProperty(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject = 0);
    ~gkStringProperty();

    virtual void apply();
    virtual void reset();

protected:
    vtkSMStringVectorProperty *outPorperty;


private:
    Ui::gkStringProperty *ui;

private slots:
    void onChangeFinished();
    void onUiChangeFinished();
};

#endif // GKSTRINGPROPERTY_H
