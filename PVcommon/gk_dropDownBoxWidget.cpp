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

//    infoProperty->AddObserver(vtkCommand::UncheckedPropertyModifiedEvent, this, &updateDropDownList);

}


gk_dropDownBoxWidget::~gk_dropDownBoxWidget()
{
    delete ui;
}

void gk_dropDownBoxWidget::apply()
{

}

void gk_dropDownBoxWidget::reset()
{

}

void gk_dropDownBoxWidget::updateDropDownList()
{
    std::cout << __FUNCTION__ << " entered" << std::endl;
}
