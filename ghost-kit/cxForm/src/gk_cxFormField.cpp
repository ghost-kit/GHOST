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


#include <sstream>
#include <qstring.h>
#include <qmap.h>


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

    this->vectorFields = vtkDataArraySelection::New();
    this->dataSources  = vtkDataArraySelection::New();
    this->scalarFields = vtkDataArraySelection::New();

    this->currentDataSourceList = vtkStringArray::New();
    this->currentScalarFieldsList = vtkStringArray::New();



    //for some reason, ParaView crashes on a QT error if I don't load an array
    //  immediately when creating the object, so I am creating a blank array here
    //  and later delete it when running RequestInformation().
    //
    //The logic of this filter requires periodic reformating of the available ararys,
    //  so this is OK for now.
    this->vectorFields->AddArray("");
    this->vectorFields->DisableAllArrays();

}

gk_cxFormField::~gk_cxFormField()
{
    this->vectorFields->Delete();
    this->dataSources->Delete();
    this->scalarFields->Delete();

    this->currentDataSourceList->Delete();
    this->currentScalarFieldsList->Delete();
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
void gk_cxFormField::SetDataSource(const char* value)
{
    std::cerr << __FUNCTION__ << " IMPLEMENTATION IN PROGRESS" << std::endl;

    if(this->dataSources->ArrayExists(value))
    {
        this->dataSources->DisableAllArrays();
        this->dataSources->EnableArray(value);
    }
    else
    {
        std::cerr << value << " Not found." << std::endl;
    }

    this->vectorFields->RemoveAllArrays();
    this->vectorFields->AddArray("test1");
    this->vectorFields->AddArray("test2");

    this->Modified();
    this->Update();
}

//===============================================//
void gk_cxFormField::PopulateVectorArrays()
{

}

//===============================================//
void gk_cxFormField::PopulateScalarArrays()
{

}

//===============================================//
vtkStringArray *gk_cxFormField::GetDataSourceInfo()
{
    std::cerr << __FUNCTION__ << " IMPLEMENTATION IN PROGRESS" << std::endl;

    //return the current data source list
    return this->currentDataSourceList;

}

//===============================================//
int gk_cxFormField::GetNumberOfTableArrays()
{
    return this->vectorFields->GetNumberOfArrays();
}

//===============================================//
const char *gk_cxFormField::GetTableArrayName(int index)
{
    return this->vectorFields->GetArrayName(index);
}

//===============================================//
int gk_cxFormField::GetTableArrayStatus(const char *name)
{
    if(this->vectorFields->ArrayIsEnabled(name))
        return 1;
    else
        return 0;
}

//===============================================//
void gk_cxFormField::SetTableArrayStatus(const char *name, int status)
{
    if(status)
    {
        this->vectorFields->EnableArray(name);
    }
    else
    {
        this->vectorFields->DisableArray(name);
    }

}

//===============================================//
void gk_cxFormField::DisableAllTableArrays()
{

    this->vectorFields->DisableAllArrays();

}

//===============================================//
void gk_cxFormField::EnableAllTableArrays()
{
    this->vectorFields->EnableAllArrays();
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
    this->splitXfield = std::string(this->vectorFields->GetArrayName(x));
}

//===============================================//
void gk_cxFormField::SetSplitY(int y)
{
    this->splitYfield = std::string(this->vectorFields->GetArrayName(y));
}

//===============================================//
void gk_cxFormField::SetSplitZ(int z)
{
    this->splitZfield = std::string(this->vectorFields->GetArrayName(z));
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
int gk_cxFormField::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkInformation* inInfo  = inputVector[0]->GetInformationObject(0);

    std::cout << "Input Information Keys: " << inInfo->GetNumberOfKeys() << std::endl;
    std::cout << "Output Information Keys: " << outInfo->GetNumberOfKeys() << std::endl;

    std::cout << "Getting Input Data..." << std::endl;

    vtkSmartPointer<vtkDataSet> input       = vtkDataSet::GetData(inputVector[0]);
    vtkSmartPointer<vtkTable>   inputTable  = vtkTable::GetData(inputVector[0]);
    vtkSmartPointer<vtkTable>   output      = vtkTable::GetData(outputVector);

    QMap<std::string, vtkPointData*> pdVector;
    QMap<std::string, vtkCellData*>  cdVector;
    QMap<std::string, vtkFieldData*> fdVector;
    QMap<std::string, vtkTable*>     tdVector;

    if(input)
    {
        pdVector["Point Data"] = input->GetPointData();
        cdVector["Cell Data"] = input->GetCellData();
        fdVector["Field Data"] = input->GetFieldData();

    }
    else
    {
        std::cout << "MultiBlock Dataset" << std::endl;
        vtkMultiBlockDataSet* inMB = vtkMultiBlockDataSet::GetData(inputVector[0]);
        if(inMB)
        {
            int numBlocks = inMB->GetNumberOfBlocks();
            std::cout << "Number of Blocks: " << numBlocks << std::endl;

            for(int x = 0; x < numBlocks; x++)
            {
                input = vtkDataSet::GetData(inMB->GetBlock(x)->GetInformation());

                if(!input)
                {
                    inputTable = vtkTable::SafeDownCast(inMB->GetBlock(x));
                }

                if(input)
                {
                    pdVector["Point Data"] = input->GetPointData();
                    cdVector["Cell Data"] = input->GetCellData();
                    fdVector["Field Data"] = input->GetFieldData();

                 }

                if(inputTable)
                {
                    QString name = QString(inMB->GetMetaData(x)->Get(vtkCompositeDataSet::NAME()));
                    tdVector[name.toStdString()] = inputTable;

                }

            }

        }

    }

    //Collected Information
    std::cout << "Number of input PDs: " << pdVector.size() << std::endl;
    std::cout << "Number of input CDs: " << cdVector.size() << std::endl;
    std::cout << "Number of input FDs: " << fdVector.size() << std::endl;
    std::cout << "Number of input TDs: " << tdVector.size() << std::endl;

    //Populate the available data sources

    //remove current items
    this->dataSources->RemoveAllArrays();
    this->currentDataSourceList->Reset();

    if(pdVector.size() > 0)
    {
        this->dataSources->AddArray(pdVector.keys()[0].c_str());
        this->currentDataSourceList->InsertNextValue(pdVector.keys()[0]);
    }

    if(cdVector.size() > 0)
    {
        this->dataSources->AddArray(cdVector.keys()[0].c_str());
        this->currentDataSourceList->InsertNextValue(cdVector.keys()[0]);
    }

    if(fdVector.size() > 0)
    {
        this->dataSources->AddArray(fdVector.keys()[0].c_str());
        this->currentDataSourceList->InsertNextValue(fdVector.keys()[0]);
    }

    if(tdVector.size() > 0)
    {
        for(int x = 0; x < tdVector.size(); x++)
        {

            this->dataSources->AddArray(tdVector.keys()[x].c_str());
            this->currentDataSourceList->InsertNextValue(tdVector.keys()[x]);

        }
    }

    return 1;
}

//===============================================//
