#include "gkPlotSource.h"


#include "vtkCommand.h"
#include "vtkAbstractArray.h"
#include "vtkCallbackCommand.h"
#include "vtkCompositeDataIterator.h"
#include "vtkDataSetAttributes.h"
#include "vtkDataArraySelection.h"
#include "vtkCompositeDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkDataArray.h"
#include "vtkDataObject.h"
#include "vtkFieldData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredGridAlgorithm.h"
#include "vtkStructuredGrid.h"
#include "vtkStructuredData.h"
#include "vtkTable.h"
#include "vtksys/SystemTools.hxx"

//===============================================//
vtkStandardNewMacro(gkPlotSource)

void gkPlotSource::PrintSelf(ostream &os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

}

vtkDoubleArray *gkPlotSource::getPlotData()
{


}

vtkDoubleArray *gkPlotSource::getPlotGrid()
{

}

gkPlotSource::gkPlotSource()
    : Superclass()
{
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);

    this->PlotData = vtkDoubleArray::New();
    this->PlotGrid = vtkDoubleArray::New();
}

gkPlotSource::~gkPlotSource()
{
    this->PlotData->Delete();
    this->PlotGrid->Delete();

}

int gkPlotSource::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

    return 1;
}

int gkPlotSource::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

    return 1;
}

int gkPlotSource::FillInputPortInformation(int port, vtkInformation *info)
{
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredGrid");

    return 1;
}

int gkPlotSource::FillOutputPortInformation(int port, vtkInformation *info)
{

    return Superclass::FillInputPortInformation(port,info);
}
