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
    this->outPorperty  = vtkSMStringVectorProperty::SafeDownCast(smproperty);
    this->smProxy = smproxy;

    connect(ui->dropDownBox, SIGNAL(activated(QString)), this, SLOT(selectionChanged(QString)));
    this->addPropertyLink(ui->dropDownBox, smproxy->GetPropertyName(smproperty),SIGNAL(activated(QString)), this->outPorperty);

    connect(this, SIGNAL(changeFinished()), this, SLOT(onChangeFinished()));

    //update the gui
    this->updateDropDownList();

    //add information observer
    this->infoObserver = vtkCallbackCommand::New();
    this->infoObserver->SetCallback(&gk_dropDownBoxWidget::infoCallback);
    this->infoObserver->SetClientData(this);
    this->infoProperty->AddObserver(vtkCommand::ModifiedEvent, this->infoObserver);

}

//===============================================//
gk_dropDownBoxWidget::~gk_dropDownBoxWidget()
{
    delete ui;
}

//===============================================//
void gk_dropDownBoxWidget::apply()
{
    std::cout << __FUNCTION__ << " entered" << std::endl;
    this->smProxy->UpdatePropertyInformation();

    Superclass::apply();

}

//===============================================//
void gk_dropDownBoxWidget::reset()
{
    std::cout << __FUNCTION__ << " Unimplemented" << std::endl;

}


//===============================================//
void gk_dropDownBoxWidget::updateDropDownList()
{
    std::cout << __FUNCTION__ << " Unimplemented" << std::endl;

    this->smProxy->UpdateSelfAndAllInputs();

    //clear the list
    ui->dropDownBox->clear();
    this->smProxy->UpdateProperty(this->smProxy->GetPropertyName(this->infoProperty));

    //update the list from the infoProperty
    int numElements = this->infoProperty->GetNumberOfElements();
    std::cout << "InfoProperty: " << this->smProxy->GetPropertyName(this->infoProperty) << std::endl;
    std::cout << "Number of Elements: " << numElements << std::endl;

    for(int x=0; x < numElements; x++)
    {
        ui->dropDownBox->addItem(this->infoProperty->GetElement(x));
    }

}

//===============================================//
void gk_dropDownBoxWidget::infoCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
    gk_dropDownBoxWidget* filter = static_cast<gk_dropDownBoxWidget*>(clientdata);
    filter->updateDropDownList();

}

//===============================================//
void gk_dropDownBoxWidget::selectionChanged(QString selection)
{
    std::cout << __FUNCTION__ << " Unimplemented" << std::endl;
    this->outPorperty->SetImmediateUpdate(1);
    this->outPorperty->SetElement(0, selection.toAscii().data());

}

void gk_dropDownBoxWidget::onChangeFinished()
{
    this->apply();
}

//
