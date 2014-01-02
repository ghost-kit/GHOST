#include "gk_cxform.h"

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
gk_cxform::gk_cxform()
: Superclass()
{

}

//===============================================//
vtkStandardNewMacro(gk_cxform)

//===============================================//
void gk_cxform::PrintSelf(ostream &os, vtkIndent indent)
{
}

//===============================================//
void gk_cxform::SetSourceSystem(int value)
{

}

//===============================================//
void gk_cxform::SetDestSystem(int value)
{

}

//===============================================//
int gk_cxform::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

    return 0;
}

//===============================================//
int gk_cxform::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

    return 0;
}

//===============================================//
int gk_cxform::FillInputPortInformation(int port, vtkInformation *info)
{

    return 0;
}

//===============================================//
int gk_cxform::FillOutputPortInformation(int port, vtkInformation *info)
{

    return 0;
}
