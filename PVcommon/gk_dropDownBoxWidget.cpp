#include "gk_dropDownBoxWidget.h"
#include "ui_gk_dropDownBoxWidget.h"
#include "vtkCommand.h"
#include "pqPropertiesPanel.h"


gk_dropDownBoxWidget::gk_dropDownBoxWidget(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject)
    : Superclass(smproxy, parentObject),
      ui(new Ui::gk_dropDownBoxWidget)
{

    std::cout << __FUNCTION__ << " entered" << std::endl;

    ui->setupUi(this);
    ui->gridLayout->setMargin(pqPropertiesPanel::suggestedMargin());
    ui->gridLayout->setHorizontalSpacing(pqPropertiesPanel::suggestedHorizontalSpacing());
    ui->gridLayout->setVerticalSpacing(pqPropertiesPanel::suggestedVerticalSpacing());

    //assign properties
    this->infoProperty = vtkSMStringVectorProperty::SafeDownCast(smproperty->GetInformationProperty());
    this->outPorperty  = vtkSMIntVectorProperty::SafeDownCast(smproperty);
    this->smProxy = smproxy;

    connect(ui->dropDownBox, SIGNAL(activated(QString)), this, SLOT(selectionChanged(QString)));

    //update the gui
    this->updateDropDownList();

}


gk_dropDownBoxWidget::~gk_dropDownBoxWidget()
{
    delete ui;
}

void gk_dropDownBoxWidget::apply()
{
    std::cout << __FUNCTION__ << " Unimplemented" << std::endl;

}

void gk_dropDownBoxWidget::reset()
{
    std::cout << __FUNCTION__ << " Unimplemented" << std::endl;

}

void gk_dropDownBoxWidget::updateDropDownList()
{
    std::cout << __FUNCTION__ << " Unimplemented" << std::endl;

    //clear the list
    ui->dropDownBox->clear();
    this->smProxy->UpdateProperty(this->infoProperty->GetXMLLabel());

    //update the list from the infoProperty
    int numElements = this->infoProperty->GetNumberOfElements();
    std::cout << "InfoProperty: " << this->infoProperty->GetXMLLabel() << std::endl;
    std::cout << "Number of Elements: " << numElements << std::endl;

    for(int x=0; x < numElements; x++)
    {
        ui->dropDownBox->addItem(this->infoProperty->GetElement(x));
    }

}

void gk_dropDownBoxWidget::selectionChanged(QString selection)
{
    std::cout << __FUNCTION__ << " Unimplemented" << std::endl;

}
