#include "gk_cxFormField.h"
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
#include "vtkImageData.h"
#include "vtkNew.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmartPointer.h"
#include "vtkImageDataToPointSet.h"
#include "vtkRectilinearGridToPointSet.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDataObjectTypes.h"

#include "ltrDateTime.h"
#include "cppxform.h"
#include <math.h>
#include <vector>
//===============================================//
vtkStandardNewMacro(gk_cxFormField)


//===============================================//
gk_cxFormField::gk_cxFormField()
{
    this->systemLookupTable.push_back(std::string("UNKNOWN"));
    this->systemLookupTable.push_back(std::string("J2000"));
    this->systemLookupTable.push_back(std::string("GEI"));
    this->systemLookupTable.push_back(std::string("GEO"));
    this->systemLookupTable.push_back(std::string("MAG"));
    this->systemLookupTable.push_back(std::string("GSE"));
    this->systemLookupTable.push_back(std::string("GSM"));
    this->systemLookupTable.push_back(std::string("SM"));
    this->systemLookupTable.push_back(std::string("RTN"));
    this->systemLookupTable.push_back(std::string("GSEQ"));
    this->systemLookupTable.push_back(std::string("HEE"));
    this->systemLookupTable.push_back(std::string("HAE"));
    this->systemLookupTable.push_back(std::string("HEEQ"));
    this->systemLookupTable.push_back(std::string("HEEQ180"));

    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);

    this->fields = vtkDataArraySelection::New();
    //TEST
    this->fields->AddArray("Test 1");
    this->fields->AddArray("Test 2");
    this->fields->EnableAllArrays();

}

//===============================================//
void gk_cxFormField::PrintSelf(ostream &os, vtkIndent indent)
{
    std::cerr << "Ghost Kit Field Transformation Filter" << std::endl;
}

//===============================================//
void gk_cxFormField::SetSourceSystem(int value)
{

    this->sourceSystem = value;
}

//===============================================//
void gk_cxFormField::SetDestSystem(int value)
{

    this->destSystem = value;
}

//===============================================//
int gk_cxFormField::GetNumberOfFieldArrays()
{
    return this->fields->GetNumberOfArrays();
}

//===============================================//
const char *gk_cxFormField::GetFieldArrayName(int index)
{
    return this->fields->GetArrayName(index);
}

//===============================================//
int gk_cxFormField::GetFieldArrayStatus(const char *name)
{
    if(this->fields->ArrayIsEnabled(name))
        return 1;
    else
        return 0;
}

//===============================================//
void gk_cxFormField::SetFieldArrayStatus(const char *name, int status)
{
    if(status)
    {
        this->fields->EnableArray(name);
    }
    else
    {
        this->fields->DisableArray(name);
    }

}

//===============================================//
void gk_cxFormField::DisableAllFieldArrays()
{

    this->fields->DisableAllArrays();

}

//===============================================//
void gk_cxFormField::EnableAllFieldArrays()
{
    this->fields->EnableAllArrays();
}

//===============================================//
void gk_cxFormField::SetManualFromSystem(int system)
{
    this->manualFrom = system;
}

//===============================================//
void gk_cxFormField::SetManualToSystem(int system)
{
    this->manualTo = system;
}

//===============================================//
void gk_cxFormField::setUseManual(int status)
{
    this->useManual = status;
}

//===============================================//
void gk_cxFormField::SetManualName(char *name)
{
    this->manualFieldName = std::string(name);
}

//===============================================//
void gk_cxFormField::SetManualOutput(double x, double y, double z)
{
    this->manualX = x;
    this->manualY = y;
    this->manualZ = z;
}

//===============================================//
void gk_cxFormField::SetSplitFromSystem(int system)
{
    this->splitFrom = system;
}

//===============================================//
void gk_cxFormField::SetSplitToSystem(int system)
{
    this->splitTo = system;
}

//===============================================//
void gk_cxFormField::setUseSplit(int status)
{
    this->useSplit = status;
}

//===============================================//
void gk_cxFormField::SetSplitFieldName(char *name)
{
    this->splitFieldName = std::string(name);
}

//===============================================//
void gk_cxFormField::SetSplitX(int x)
{
    this->splitXfield = std::string(this->fields->GetArrayName(x));
}

//===============================================//
void gk_cxFormField::SetSplitY(int y)
{
    this->splitYfield = std::string(this->fields->GetArrayName(y));
}

//===============================================//
void gk_cxFormField::SetSplitZ(int z)
{
    this->splitZfield = std::string(this->fields->GetArrayName(z));
}

//===============================================//
void gk_cxFormField::SetManualInput(double x, double y, double z)
{
    this->manualX = x;
    this->manualY = y;
    this->manualZ = z;
}



//===============================================//
int gk_cxFormField::FillInputPortInformation(
    int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
  return 1;
}

//===============================================//
int gk_cxFormField::RequestDataObject(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // Output type is same as input
  vtkDataObject *input = vtkDataObject::GetData(inputVector[0], 0);
  if (input)
    {
    const char* outputType = "vtkTable";
    if (input->IsA("vtkCompositeDataSet"))
      {
      outputType = "vtkMultiBlockDataSet";
      }

    // for each output
    for (int i=0; i < this->GetNumberOfOutputPorts(); ++i)
      {
      vtkInformation* info = outputVector->GetInformationObject(i);
      vtkDataObject *output = info->Get(vtkDataObject::DATA_OBJECT());

      if (!output || !output->IsA(outputType))
        {
        vtkDataObject* newOutput =
          vtkDataObjectTypes::NewDataObject(outputType);
        info->Set(vtkDataObject::DATA_OBJECT(), newOutput);
        newOutput->Delete();
        this->GetOutputPortInformation(0)->Set(
          vtkDataObject::DATA_EXTENT_TYPE(), newOutput->GetExtentType());
        }
      }
    return 1;
    }

  return 0;
}


//===============================================//
int gk_cxFormField::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

    std::cerr << "Requesting Data..." << std::endl;

    return 1;
}

//===============================================//
