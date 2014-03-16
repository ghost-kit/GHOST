#include "gk_selectionBoxWidget.h"
#include "ui_gk_selectionBoxWidget.h"

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

#include <vtkSMProxy.h>
#include <vtkSMProperty.h>
#include <vtkSMSessionProxyManager.h>
#include <vtkSMGlobalPropertiesManager.h>
#include <vtkSMStringVectorProperty.h>
#include <vtkSMDoubleVectorProperty.h>

#include <iomanip>

#include <Qt>
#include <QListWidgetItem>
#include "qlist.h"
#include "QStringList"
#include "qstring.h"
#include "QTreeWidget"
#include "QWidget"
#include "QTreeView"

#define COLUMN_COUNT 1

gk_selectionBoxWidget::gk_selectionBoxWidget(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject)
    : Superclass(smproxy, parentObject),
      ui(new Ui::gk_selectionBoxWidget)
{
    //get input info property
    this->inInfoProperty =  vtkSMStringVectorProperty::SafeDownCast(smproperty->GetInformationProperty());
    this->currentProperty = vtkSMStringVectorProperty::SafeDownCast(smproperty);

    //get title of box
    QString title(this->currentProperty->GetXMLLabel());

    //set up the GUI
    ui->setupUi(this);
    ui->gridLayout->setMargin(pqPropertiesPanel::suggestedMargin());
    ui->gridLayout->setHorizontalSpacing(pqPropertiesPanel::suggestedHorizontalSpacing());
    ui->gridLayout->setVerticalSpacing(pqPropertiesPanel::suggestedVerticalSpacing());
    ui->varList->setColumnCount(COLUMN_COUNT);
    ui->varList->setHeaderLabel(title);

    this->processProperty();

    //add information observer
    this->infoObserver = vtkCallbackCommand::New();
    this->infoObserver->SetCallback(&gk_selectionBoxWidget::infoCallback);
    this->infoObserver->SetClientData(this);
    this->inInfoProperty->AddObserver(vtkCommand::ModifiedEvent, this->infoObserver);

    // dont show label
    this->setShowLabel(false);
}

gk_selectionBoxWidget::~gk_selectionBoxWidget()
{
    delete ui;
}

void gk_selectionBoxWidget::apply()
{
    proxy()->UpdatePropertyInformation();
    Superclass::apply();

}

void gk_selectionBoxWidget::reset()
{

    Superclass::reset();
}

void gk_selectionBoxWidget::processProperty()
{

    //clear UI
    ui->varList->clear();

    //update the property
    proxy()->UpdatePropertyInformation();

    //process the property
    int numElements = this->inInfoProperty->GetNumberOfElements();
    if(numElements % 2 == 0)
    {
        for(int x=0; x < numElements; x += 2 )
        {

            QString itemName =  QString(this->inInfoProperty->GetElement(x));
            QString itemState = QString(this->inInfoProperty->GetElement(x+1));

            pqTreeWidgetItem* Item = new pqTreeWidgetItem;
            Item->setText(0,itemName);
            if(itemState == "1")
            {
                Item->setCheckState(0, Qt::Checked);
            }
            else
            {
                Item->setCheckState(0, Qt::Unchecked);
            }

            ui->varList->addTopLevelItem(Item);

        }
    }
    else
    {
        //vtkErrorMacro( << "Error Parsing Selection Box Widget Information Property\nThe Format of the property should be odd Row: name, even Row: State");
        std::cerr << "Error Parsing Selection Box Widget Information Property\nThe Format of the property should be odd Row: name, even Row: State" << std::endl;

    }
}

void gk_selectionBoxWidget::infoCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
    gk_selectionBoxWidget* filter = static_cast<gk_selectionBoxWidget*>(clientdata);
    filter->processProperty();

}
