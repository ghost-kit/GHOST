#include "gkPlotSourcePanel.h"




gkPlotSourcePanel::gkPlotSourcePanel(vtkSMProxy *smproxy, vtkSMProperty *smproperty, QWidget *parentObject)
{
    //link properties
    //TODO: set required properties
    //TODO: Use hints to get proper properties?
    this->infoObserver = vtkCallbackCommand::New();
    this->infoObserver->SetCallback(&gkPlotSourcePanel::infoCallback);
    this->infoObserver->SetClientData(this);
    //TODO: Add property observers
}

gkPlotSourcePanel::~gkPlotSourcePanel()
{
    this->infoObserver->Delete();

}

void gkPlotSourcePanel::apply()
{
    //TODO: design update process that updates all of the points
    proxy()->UpdatePropertyInformation();
    Superclass::apply();

}

void gkPlotSourcePanel::updateGrid()
{
    //TODO: This will be the function that updates the grid
}

void gkPlotSourcePanel::infoCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
    //TODO: if the property changes, cancle updates, and start again

}


//TODO: Changed mind... simplify... utilize the single property with command to update the array
//TODO: with extents
