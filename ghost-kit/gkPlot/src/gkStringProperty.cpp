#include "gkStringProperty.h"
#include "ui_gkStringProperty.h"

gkStringProperty::gkStringProperty(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject) :
    Superclass(smproxy, parentObject),
    ui(new Ui::gkStringProperty)
{
    ui->setupUi(this);
    ui->gridLayout->setMargin(pqPropertiesPanel::suggestedMargin());
    ui->gridLayout->setHorizontalSpacing(pqPropertiesPanel::suggestedHorizontalSpacing());
    ui->gridLayout->setVerticalSpacing(pqPropertiesPanel::suggestedVerticalSpacing());

    this->outPorperty  = vtkSMStringVectorProperty::SafeDownCast(smproperty);
    this->addPropertyLink(ui->string, smproxy->GetPropertyName(smproperty),SIGNAL(textEdited(QString)), this->outPorperty);

    connect(this, SIGNAL(changeFinished()), this, SLOT(onChangeFinished()));
    connect(ui->string, SIGNAL(editingFinished()), this, SLOT(onUiChangeFinished()));

    //get default value
    this->ui->string->setText(this->outPorperty->GetDefaultValue(0));
}

gkStringProperty::~gkStringProperty()
{
    delete ui;
}

void gkStringProperty::apply()
{
    proxy()->UpdatePropertyInformation();
    Superclass::apply();
}

void gkStringProperty::reset()
{

    Superclass::reset();
}

void gkStringProperty::onChangeFinished()
{
    this->apply();
}

void gkStringProperty::onUiChangeFinished()
{
    QString text = ui->string->text();
    this->outPorperty->SetImmediateUpdate(1);
    outPorperty->SetElement(0, text.toAscii().data());
    emit this->changeFinished();
}
