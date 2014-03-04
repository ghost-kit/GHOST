#ifndef GK_DROPDOWNBOXWIDGET_H
#define GK_DROPDOWNBOXWIDGET_H

//LOCAL
#include "pqPropertyWidget.h"
#include "pqOutputPort.h"

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

namespace Ui {
class gk_dropDownBoxWidget;
}

class gk_dropDownBoxWidget : public pqPropertyWidget
{
    Q_OBJECT
    typedef pqPropertyWidget Superclass;

public:
    explicit gk_dropDownBoxWidget(QWidget *parent = 0);
    ~gk_dropDownBoxWidget();

    virtual void apply();
    virtual void reset();

protected:
    vtkSMStringVectorProperty *availableFieldsNames;
    vtkSMStringVectorProperty *selectedFieldName;
    vtkSMIntVectorProperty    *selectedField;

private:
    Ui::gk_dropDownBoxWidget *ui;
};

#endif // GK_DROPDOWNBOXWIDGET_H
