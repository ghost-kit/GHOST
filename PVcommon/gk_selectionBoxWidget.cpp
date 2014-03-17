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

    //add information observer
    this->infoObserver = vtkCallbackCommand::New();
    this->infoObserver->SetCallback(&gk_selectionBoxWidget::infoCallback);
    this->infoObserver->SetClientData(this);
    this->inInfoProperty->AddObserver(vtkCommand::ModifiedEvent, this->infoObserver);

    //this cuases a slight problem... it will return a null selection with a -1 status to the
    //  calling program if nothing has changed.  This needs to be checked for in the
    //  calling program.
    this->mostRecentSelection.clear();
    this->mostRecentStatus = -1;

    //connection
    connect(ui->varList, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(selectionChanged(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(changeFinished()), this, SLOT(onSelectionChangeFinished()));

    this->addPropertyLink(ui->varList, smproxy->GetPropertyName(smproperty), SIGNAL(itemChanged(QTreeWidgetItem*, int)),this->currentProperty);

    // process the properites
    this->processProperty();

    // dont show label
    this->setShowLabel(false);
}

gk_selectionBoxWidget::~gk_selectionBoxWidget()
{
    delete ui;
}

void gk_selectionBoxWidget::apply()
{

    //if no current selection, need to choose one, else we get undefined behavoir.
    if(this->mostRecentSelection == "")
    {
        if(ui->varList->topLevelItemCount())
        {
            this->mostRecentSelection = QString(ui->varList->topLevelItem(0)->text(0));
            this->mostRecentStatus    = ui->varList->topLevelItem(0)->checkState(0);
        }
    }

    //configure the output information
    //this will be called every time the selection changes
    //therefore we only need send what changes with its status
    this->currentProperty->SetElement(0, this->mostRecentSelection.toAscii().data());
    this->currentProperty->SetElement(1, QString::number(this->mostRecentStatus).toAscii().data());

    if(ui->varList->topLevelItemCount())
    {
        proxy()->UpdatePropertyInformation();
        Superclass::apply();
    }
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


//slots
void gk_selectionBoxWidget::selectionChanged(QTreeWidgetItem *item, int status)
{
    this->mostRecentSelection = QString(item->text(0));
    this->mostRecentStatus    = item->checkState(0);

    emit this->changeFinished();
}

void gk_selectionBoxWidget::onSelectionChangeFinished()
{
    this->apply();
}
