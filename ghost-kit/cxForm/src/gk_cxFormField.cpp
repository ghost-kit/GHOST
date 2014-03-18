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
#include <qstringlist.h>
#include <qlist.h>
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
    //set up systems
    this->availableSystemList = vtkStringArray::New();

    this->availableSystemList->InsertNextValue("UNKNOWN");
    this->availableSystemList->InsertNextValue("J2000");
    this->availableSystemList->InsertNextValue("GEI");
    this->availableSystemList->InsertNextValue("GEO");
    this->availableSystemList->InsertNextValue("MAG");
    this->availableSystemList->InsertNextValue("GSE");
    this->availableSystemList->InsertNextValue("GSM");
    this->availableSystemList->InsertNextValue("SM");
    this->availableSystemList->InsertNextValue("RTN");
    this->availableSystemList->InsertNextValue("GSEQ");
    this->availableSystemList->InsertNextValue("HEE");
    this->availableSystemList->InsertNextValue("HAE");
    this->availableSystemList->InsertNextValue("HEEQ");
    this->availableSystemList->InsertNextValue("HEEQ180");

    //setup ports
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);

    //initialize array selectors
    this->vectorFields = vtkDataArraySelection::New();
    this->dataSources  = vtkDataArraySelection::New();
    this->scalarFields = vtkDataArraySelection::New();

    //initialize current sources
    this->currentDataSourceList = vtkStringArray::New();
    this->currentScalarFieldsList = vtkStringArray::New();
    this->currentVectorFieldsList = vtkStringArray::New();

    //Set up callbacks
    this->SourceObserver = vtkCallbackCommand::New();
    this->ScalarObserver = vtkCallbackCommand::New();
    this->VectorObserver = vtkCallbackCommand::New();
    this->AvailableSystemObserver = vtkCallbackCommand::New();

    this->SourceObserver->SetCallback(&gk_cxFormField::SourceCallback);
    this->ScalarObserver->SetCallback(&gk_cxFormField::ScalarCallback);
    this->VectorObserver->SetCallback(&gk_cxFormField::VectorCallback);
    this->AvailableSystemObserver->SetCallback(&gk_cxFormField::AvailableSystemCallback);

    this->SourceObserver->SetClientData(this);
    this->ScalarObserver->SetClientData(this);
    this->VectorObserver->SetClientData(this);
    this->AvailableSystemObserver->SetClientData(this);

    this->vectorFields->AddObserver(vtkCommand::ModifiedEvent, this->VectorObserver);
    this->scalarFields->AddObserver(vtkCommand::ModifiedEvent, this->ScalarObserver);
    this->dataSources->AddObserver(vtkCommand::ModifiedEvent, this->SourceObserver);
    this->availableSystemList->AddObserver(vtkCommand::ModifiedEvent, this->AvailableSystemObserver);

}


//===============================================//
void gk_cxFormField::generateVectorStringArray()
{
    //clear current list
    this->currentVectorFieldsList->Reset();

    //generate new list and satuses
    int numArrays = this->GetNumberOfTableArrays();
    for(int x = 0; x < numArrays; x++)
    {
        int status = this->GetTableArrayStatus(this->GetTableArrayName(x));
        this->currentVectorFieldsList->InsertNextValue(this->GetTableArrayName(x));
        this->currentVectorFieldsList->InsertNextValue((status == 0) ? "0" : "1" );
    }

    this->Modified();
}


//===============================================//
gk_cxFormField::~gk_cxFormField()
{
    this->vectorFields->Delete();
    this->dataSources->Delete();
    this->scalarFields->Delete();

    this->currentDataSourceList->Delete();
    this->currentScalarFieldsList->Delete();

    this->SourceObserver->Delete();
    this->ScalarObserver->Delete();
    this->VectorObserver->Delete();
}

//===============================================//
void gk_cxFormField::PrintSelf(ostream &os, vtkIndent indent)
{
    std::cerr << "Ghost Kit Field Transformation Filter" << std::endl;
}

//===============================================//
void gk_cxFormField::SetSourceSystem(const char* value)
{

    this->sourceSystem = vtkStdString(value);
}

//===============================================//
vtkStringArray *gk_cxFormField::GetSourceInfo()
{

    return this->availableSystemList;
}

//===============================================//
void gk_cxFormField::SetDestSystem(const char* value)
{

    this->destSystem = vtkStdString(value);
}

//===============================================//
vtkStringArray *gk_cxFormField::GetDestinationInfo()
{

    return this->availableSystemList;
}

//===============================================//
void gk_cxFormField::SetDataSource(const char* value)
{

    //set the data source we are using
    this->DataSource = vtkStdString(value);

    //mark in the tracker...
    //  TODO: remove the data source tracker IFF we decied to NOT use
    //          multiple source selection ONLY
    if(this->dataSources->ArrayExists(value))
    {
        this->dataSources->DisableAllArrays();
        this->dataSources->EnableArray(value);
    }
    else
    {
        std::cerr << value << " Not found." << std::endl;
    }


    //populate the data arrays
    this->PopulateArrays();

}

//===============================================//
void gk_cxFormField::PopulateArrays()
{

    this->currentScalarFieldsList->Reset();
    this->currentVectorFieldsList->Reset();

    this->scalarFields->RemoveAllArrays();
    this->vectorFields->RemoveAllArrays();

    //process Scalars
    if(AvailableScalarFields.contains(QString::fromStdString(this->DataSource)))
    {
        //process scalars
        for(int x = 0; x < this->AvailableScalarFields[QString::fromStdString(this->DataSource)].size(); x++ )
        {
            this->currentScalarFieldsList->InsertNextValue(this->AvailableScalarFields[QString::fromStdString(this->DataSource)][x].toAscii().data());
            this->scalarFields->AddArray(this->AvailableScalarFields[QString::fromStdString(this->DataSource)][x].toAscii().data());
        }

    }

    if(AvailableVectorFields.contains(QString::fromStdString(this->DataSource)))
    {
        //process vectors
        for(int x = 0; x < this->AvailableVectorFields[QString::fromStdString(this->DataSource)].size(); x++ )
        {
            this->currentVectorFieldsList->InsertNextValue(this->AvailableVectorFields[QString::fromStdString(this->DataSource)][x].toAscii().data());
            this->vectorFields->AddArray(this->AvailableVectorFields[QString::fromStdString(this->DataSource)][x].toAscii().data());

        }
    }

}


//===============================================//
vtkStringArray *gk_cxFormField::GetDataSourceInfo()
{

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
void gk_cxFormField::SetArraySelectionStatus(const char *name, int status)
{

    if(vectorFields->ArrayExists(name))
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
void gk_cxFormField::SetManualFromSystem(const char* system)
{
    this->manualFrom = vtkStdString(system);
}

//===============================================//
void gk_cxFormField::SetManualToSystem(const char* system)
{
    this->manualTo = vtkStdString(system);
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
void gk_cxFormField::SetSplitFromSystem(const char*  system)
{
    this->splitFrom = vtkStdString(system);
}

//===============================================//
void gk_cxFormField::SetSplitToSystem(const char*  system)
{
    this->splitTo = vtkStdString(system);
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
void gk_cxFormField::SetSplitX(const char *x)
{
    this->splitXfield = vtkStdString(x);
}

//===============================================//
void gk_cxFormField::SetSplitY(const char *y)
{
    this->splitYfield = vtkStdString(y);
}

//===============================================//
void gk_cxFormField::SetSplitZ(const char *z)
{
    this->splitZfield = vtkStdString(z);
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
vtkDoubleArray* gk_cxFormField::convertDoubleData(QString name, vtkDoubleArray* oldData, DateTime inputDate)
{
    vtkDoubleArray* newData = vtkDoubleArray::New();
    newData->SetName(name.toAscii().data());
    newData->SetNumberOfComponents(3);
    //convert the data
    int numElements = oldData->GetNumberOfTuples();

    std::cout << "Num Elements: " << numElements << " " << __FUNCTION__ << std::endl << std::flush;

    for(int y = 0; y < numElements; y++)
    {

        //get element
        double* tuple = new double[3];
        tuple = oldData->GetTuple3(y);

        double* xtuple = new double[3];


        //convert element
        cppForm::cppxform xform(inputDate, this->sourceSystem.c_str(), tuple);
        xtuple = xform.cxForm(this->destSystem.c_str());
        newData->InsertNextTuple(xtuple);

    }

    return newData;
}

vtkFloatArray* gk_cxFormField::convertFloatData(QString name, vtkFloatArray* oldData, DateTime inputDate)
{
    vtkFloatArray* newData = vtkFloatArray::New();
    newData->SetName(name.toAscii().data());
    newData->SetNumberOfComponents(3);
    //convert the data
    int numElements = oldData->GetNumberOfTuples();

    std::cout << "Num Elements: " << numElements << " " << __FUNCTION__ << std::endl << std::flush;

    for(int y = 0; y < numElements; y++)
    {
        //get element
        double* tuple = new double[3];
        tuple = oldData->GetTuple3(y);

        double* xtuple = new double[3];


        //convert element
        cppForm::cppxform xform(inputDate, this->sourceSystem.c_str(), tuple);
        xtuple = xform.cxForm(this->destSystem.c_str());
        newData->InsertNextTuple(xtuple);

    }

    return newData;
}

vtkDoubleArray *gk_cxFormField::getAsDouble(QString ArrayName, vtkFieldData *dataSource)
{
    vtkSmartPointer<vtkDoubleArray> data = vtkDoubleArray::SafeDownCast(dataSource->GetAbstractArray(ArrayName.toAscii().data()));

    if(data) return data.GetPointer();

    vtkSmartPointer<vtkFloatArray> floatData = vtkFloatArray::SafeDownCast(dataSource->GetAbstractArray(ArrayName.toAscii().data()));

    if(floatData)
    {
        vtkDoubleArray* outData = vtkDoubleArray::New();
        outData->SetName(ArrayName.toAscii().data());
        outData->SetNumberOfComponents(floatData->GetNumberOfComponents());

        int numberTuples = floatData->GetNumberOfTuples();
        for(int x = 0; x < numberTuples; x++)
        {
            double tuple[3] = {0,0,0};
            floatData->GetTuple(x, tuple);
            outData->InsertNextTuple(tuple);
        }

        return outData;

    }

    return NULL;

}

vtkDoubleArray *gk_cxFormField::getAsDouble(QString ArrayName, vtkTable *dataSource)
{
    vtkSmartPointer<vtkDoubleArray> data = vtkDoubleArray::SafeDownCast(dataSource->GetColumnByName(ArrayName.toAscii().data()));

    if(data) return data.GetPointer();

    vtkSmartPointer<vtkFloatArray> floatData = vtkFloatArray::SafeDownCast(dataSource->GetColumnByName(ArrayName.toAscii().data()));

    if(floatData)
    {
        vtkDoubleArray* outData = vtkDoubleArray::New();
        outData->SetName(ArrayName.toAscii().data());
        outData->SetNumberOfComponents(floatData->GetNumberOfComponents());

        int numberTuples = floatData->GetNumberOfTuples();
        for(int x = 0; x < numberTuples; x++)
        {
            double tuple[3] = {0,0,0};
            floatData->GetTuple(x, tuple);
            outData->InsertNextTuple(tuple);
        }

        return outData;

    }

    return NULL;
}

void gk_cxFormField::convertVectorData(DateTime inputDate, QString name, vtkTable* outputTable, vtkFieldData* currentData)
{
    vtkSmartPointer<vtkDoubleArray> oldData;
    oldData = vtkDoubleArray::SafeDownCast(currentData->GetAbstractArray(name.toAscii().data()));

    if(oldData)
    {
        //double arrays
        vtkDoubleArray* newData = convertDoubleData(name, oldData, inputDate);
        outputTable->AddColumn(newData);
    }
    else
    {
        //float arrays
        vtkSmartPointer<vtkFloatArray> oldData;
        oldData = vtkFloatArray::SafeDownCast(currentData->GetAbstractArray(name.toAscii().data()));

        if(oldData)
        {
            vtkFloatArray* newData = convertFloatData(name, oldData, inputDate);
            outputTable->AddColumn(newData);

        }
    }
}

void gk_cxFormField::convertVectorData(DateTime inputDate, QString name, vtkTable *outputTable, vtkTable *currentData)
{
    vtkSmartPointer<vtkDoubleArray> oldData;
    oldData = vtkDoubleArray::SafeDownCast(currentData->GetColumnByName(name.toAscii().data()));

    if(oldData)
    {
        //double arrays
        vtkDoubleArray* newData = convertDoubleData(name, oldData, inputDate);
        outputTable->AddColumn(newData);
    }
    else
    {
        //float arrays
        vtkSmartPointer<vtkFloatArray> oldData;
        oldData = vtkFloatArray::SafeDownCast(currentData->GetColumnByName(name.toAscii().data()));

        std::cout << "Got Old Data" << std::endl;

        if(oldData)
        {
            std::cout << "Attempting to to call convert Float Data" << std::endl;
            vtkFloatArray* newData = convertFloatData(name, oldData, inputDate);

            if(newData) outputTable->AddColumn(newData);
            else std::cerr << "Failed to Build Column for " << name.toAscii().data() << std::endl;

        }
    }
}

void gk_cxFormField::convertSplitData(DateTime inputDate, QString nameX, QString nameY, QString nameZ, vtkTable *outputTable, vtkFieldData *currentData)
{
    vtkDoubleArray* X = this->getAsDouble(nameX, currentData);
    vtkDoubleArray* Y = this->getAsDouble(nameY, currentData);
    vtkDoubleArray* Z = this->getAsDouble(nameZ, currentData);

    //build new array
    vtkDoubleArray* newVector = vtkDoubleArray::New();
    newVector->SetName(this->splitFieldName.c_str());
    newVector->SetNumberOfComponents(3);

    int numElements = 0;
    if(X && Y && Z)
    {
        numElements = X->GetNumberOfTuples();

        for(int x = 0; x < numElements; x++)
        {
            newVector->InsertNextTuple3(X->GetValue(x), Y->GetValue(x), Z->GetValue(x));
        }

        vtkDoubleArray* xformd = this->convertDoubleData(QString(this->splitFieldName), newVector, inputDate);
        outputTable->AddColumn(xformd);
        xformd->Delete();
    }

    newVector->Delete();

}

void gk_cxFormField::convertSplitData(DateTime inputDate, QString nameX, QString nameY, QString nameZ, vtkTable *outputTable, vtkTable *currentData)
{
    vtkDoubleArray* X = this->getAsDouble(nameX, currentData);
    vtkDoubleArray* Y = this->getAsDouble(nameY, currentData);
    vtkDoubleArray* Z = this->getAsDouble(nameZ, currentData);

    //build new array
    vtkDoubleArray* newVector = vtkDoubleArray::New();
    newVector->SetName(this->splitFieldName.c_str());
    newVector->SetNumberOfComponents(3);

    int numElements = 0;
    if(X && Y && Z)
    {
        numElements = X->GetNumberOfTuples();

        for(int x = 0; x < numElements; x++)
        {
            newVector->InsertNextTuple3(X->GetValue(x), Y->GetValue(x), Z->GetValue(x));
        }

        vtkDoubleArray* xformd = this->convertDoubleData(QString(this->splitFieldName), newVector, inputDate);
        outputTable->AddColumn(xformd);
        xformd->Delete();
    }

    newVector->Delete();

}

int gk_cxFormField::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

    //get inpput and output inforamation
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkInformation* inInfo  = inputVector[0]->GetInformationObject(0);

    std::cout << "Output: " << __FUNCTION__ << std::endl;

    vtkTable* outputTable = vtkTable::GetData(outputVector);

    double mjd = 0.0;

    //get time request data
    if(inInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()))
    {
        mjd = inInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
    }

    std::cout << "MJD: " << __FUNCTION__ << std::endl;

    //create data time element
    DateTime inputDate(mjd);

    //get active data set names
    QStringList pdKeys = this->pdVector.keys();
    QStringList cdKeys = this->cdVector.keys();
    QStringList fdKeys = this->fdVector.keys();
    QStringList tdKeys = this->tdVector.keys();

    std::cout << "keys: " << __FUNCTION__ << std::endl;


    //check pointData for active data set
    if(pdKeys.contains(QString(this->DataSource)))
    {
        vtkPointData* currentData = this->pdVector[QString(this->DataSource)];

        std::cout << "currentData: " << __FUNCTION__ << std::endl;

        //check vectors
        int numVectors = this->vectorFields->GetNumberOfArrays();
        for(int x = 0; x < numVectors; x++)
        {
            QString name = QString(this->vectorFields->GetArrayName(x));

            //skip if array not enabled
            if(!this->vectorFields->ArrayIsEnabled(name.toAscii().data())) continue;
            std::cout << "Name: " << name.toAscii().data() << " " << __FUNCTION__ << std::endl;

            //get the data from the variable and convert to new variable
            this->convertVectorData(inputDate, name, outputTable, currentData);
        }

        //check for split transform
        if(this->useSplit)
        {
            this->convertSplitData(inputDate, QString(this->splitXfield), QString(this->splitYfield), QString(this->splitZfield), outputTable, currentData);

        }


    }

    //check cell data for active data set
    if(cdKeys.contains(QString(this->DataSource)))
    {
        vtkCellData* currentData = this->cdVector[QString(this->DataSource)];

        int numVectors = this->vectorFields->GetNumberOfArrays();
        for(int x = 0; x < numVectors; x++)
        {
            QString name = QString(this->vectorFields->GetArrayName(x));

            //skip if array not enabled
            if(!this->vectorFields->ArrayIsEnabled(name.toAscii().data())) continue;
            std::cout << "Name: " << name.toAscii().data() << " " << __FUNCTION__ << std::endl;

            //get the data from the variable and convert to new variable
            this->convertVectorData(inputDate, name, outputTable, currentData);

        }

        //check for split transform
        if(this->useSplit)
        {
            this->convertSplitData(inputDate, QString(this->splitXfield), QString(this->splitYfield), QString(this->splitZfield), outputTable, currentData);

        }

    }

    //check field data for active data set
    if(fdKeys.contains(QString(this->DataSource)))
    {
        vtkFieldData* currentData = this->fdVector[QString(this->DataSource)];

        int numVectors = this->vectorFields->GetNumberOfArrays();
        for(int x = 0; x < numVectors; x++)
        {
            QString name = QString(this->vectorFields->GetArrayName(x));

            //skip if array not enabled
            if(!this->vectorFields->ArrayIsEnabled(name.toAscii().data())) continue;
            std::cout << "Name: " << name.toAscii().data() << " " << __FUNCTION__ << std::endl;

            //get the data from the variable and convert to new variable
            this->convertVectorData(inputDate, name, outputTable, currentData);

        }

        //check for split transform
        if(this->useSplit)
        {
            this->convertSplitData(inputDate, QString(this->splitXfield), QString(this->splitYfield), QString(this->splitZfield), outputTable, currentData);

        }
    }

    //check table data for active data set
    if(tdKeys.contains(QString(this->DataSource)))
    {
        vtkTable* currentData = this->tdVector[QString(this->DataSource)];

        int numVectors = this->vectorFields->GetNumberOfArrays();
        for(int x = 0; x < numVectors; x++)
        {
            QString name = QString(this->vectorFields->GetArrayName(x));

            //skip if array not enabled
            if(!this->vectorFields->ArrayIsEnabled(name.toAscii().data())) continue;
            std::cout << "Name: " << name.toAscii().data() << " " << __FUNCTION__ << std::endl;

            //get the data from the variable and convert to new variable
            this->convertVectorData(inputDate, name, outputTable, currentData);

        }

        //check for split transform
        if(this->useSplit)
        {
            this->convertSplitData(inputDate, QString(this->splitXfield), QString(this->splitYfield), QString(this->splitZfield), outputTable, currentData);

        }
    }


    //computer and add the manual values (if requested)
    //  this is more intended for the Python Interface, but will work just fine with the GUI
    if(this->useManual)
    {
        //manual xform requested

        //create output array
        vtkDoubleArray* newArray = vtkDoubleArray::New();

        //get the input vector
        double inputVector[3] = {this->manualX, this->manualY, this->manualZ};

        //setup the xform
        cppForm::cppxform xform(inputDate, this->sourceSystem.c_str(), inputVector);

        //configure and add value to output array
        newArray->SetName(this->manualFieldName.c_str());
        newArray->SetNumberOfComponents(3);
        newArray->InsertNextTuple(xform.cxForm(this->destSystem));

        //add to output table
        outputTable->AddColumn(newArray);

    }


    return 1;
}

//===============================================//
int gk_cxFormField::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
    this->dataSources->RemoveAllArrays();
    this->AvailableVectorFields.clear();
    this->AvailableScalarFields.clear();

    //clear out old gathered information
    this->pdVector.clear();
    this->cdVector.clear();
    this->fdVector.clear();
    this->tdVector.clear();

    //get inpput and output inforamation
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkInformation* inInfo  = inputVector[0]->GetInformationObject(0);

    vtkSmartPointer<vtkDataSet> input       = vtkDataSet::GetData(inputVector[0]);
    vtkSmartPointer<vtkTable>   inputTable  = vtkTable::GetData(inputVector[0]);
    vtkSmartPointer<vtkTable>   output      = vtkTable::GetData(outputVector);

    if(input)
    {
        if(input->GetPointData()->GetNumberOfArrays() > 0)
            pdVector[GK_POINTDATASTR] = input->GetPointData();

        if(input->GetCellData()->GetNumberOfArrays() > 0)
            cdVector[GK_CELLDATASTR] = input->GetCellData();

        if(input->GetFieldData()->GetNumberOfArrays() > 0)
            fdVector[GK_FIELDDATASTR] = input->GetFieldData();

    }
    else
    {
        vtkMultiBlockDataSet* inMB = vtkMultiBlockDataSet::GetData(inputVector[0]);
        if(inMB)
        {
            int numBlocks = inMB->GetNumberOfBlocks();
            QString name;


            for(int x = 0; x < numBlocks; x++)
            {
                input = vtkDataSet::GetData(inMB->GetBlock(x)->GetInformation());
                name = QString(inMB->GetMetaData(x)->Get(vtkCompositeDataSet::NAME()));

                if(!input)
                {
                    inputTable = vtkTable::SafeDownCast(inMB->GetBlock(x));
                }

                if(input)
                {
                    if(input->GetPointData()->GetNumberOfArrays() > 0)
                        pdVector[QString(name + GK_POINTDATASTR)] = input->GetPointData();

                    if(input->GetCellData()->GetNumberOfArrays() > 0)
                        cdVector[QString(name + GK_CELLDATASTR)] = input->GetCellData();

                    if(input->GetFieldData()->GetNumberOfArrays() > 0)
                        fdVector[QString(name + GK_FIELDDATASTR)] = input->GetFieldData();

                }

                if(inputTable)
                {
                    tdVector[name] = inputTable;

                }

            }

        }

    }

    //remove current items
    this->dataSources->RemoveAllArrays();
    this->currentDataSourceList->Reset();


    if(pdVector.size() > 0)
    {
        //TODO: BUG: Fix this to load ALL PD and CD and FD fields from MB data sets
        this->dataSources->AddArray(pdVector.keys()[0].toAscii().data());
        this->currentDataSourceList->InsertNextValue(pdVector.keys()[0].toAscii().data());

        for(int x = 0; x < pdVector[GK_POINTDATASTR]->GetNumberOfArrays(); x++)
        {
            if(pdVector[GK_POINTDATASTR]->GetAbstractArray(x)->GetNumberOfComponents() == 3)
            {
                //vector we can transform
                this->AvailableVectorFields[GK_POINTDATASTR].push_back(QString(pdVector[GK_POINTDATASTR]->GetAbstractArray(x)->GetName()));

            }
            else if(pdVector[GK_POINTDATASTR]->GetAbstractArray(x)->GetNumberOfComponents() == 1
                    && (pdVector[GK_POINTDATASTR]->GetAbstractArray(x)->GetDataType() == VTK_DOUBLE
                        || pdVector[GK_POINTDATASTR]->GetAbstractArray(x)->GetDataType() == VTK_FLOAT))
            {
                //component we can use for scalar
                this->AvailableScalarFields[GK_POINTDATASTR].push_back(QString(pdVector[GK_POINTDATASTR]->GetAbstractArray(x)->GetName()));

            }
        }
    }

    if(cdVector.size() > 0)
    {
        //TODO: BUG: Fix this to load ALL PD and CD and FD fields from MB data sets
        this->dataSources->AddArray(cdVector.keys()[0].toAscii().data());
        this->currentDataSourceList->InsertNextValue(cdVector.keys()[0].toAscii().data());

        for(int x = 0; x < cdVector[GK_CELLDATASTR]->GetNumberOfArrays(); x++)
        {
            if(cdVector[GK_CELLDATASTR]->GetAbstractArray(x)->GetNumberOfComponents() == 3)
            {
                //vector we can transform
                this->AvailableVectorFields[GK_CELLDATASTR].push_back(QString(cdVector[GK_CELLDATASTR]->GetAbstractArray(x)->GetName()));

            }
            else if(cdVector[GK_CELLDATASTR]->GetAbstractArray(x)->GetNumberOfComponents() == 1
                    && (cdVector[GK_CELLDATASTR]->GetAbstractArray(x)->GetDataType() == VTK_DOUBLE
                        || cdVector[GK_CELLDATASTR]->GetAbstractArray(x)->GetDataType() == VTK_FLOAT))
            {
                //component we can use for scalar
                this->AvailableScalarFields[GK_CELLDATASTR].push_back(QString(cdVector[GK_CELLDATASTR]->GetAbstractArray(x)->GetName()));

            }
        }
    }

    if(fdVector.size() > 0)
    {
        //TODO: BUG: Fix this to load ALL PD and CD and FD fields from MB data sets
        this->dataSources->AddArray(fdVector.keys()[0].toAscii().data());
        this->currentDataSourceList->InsertNextValue(fdVector.keys()[0].toAscii().data());

        for(int x = 0; x < fdVector[GK_FIELDDATASTR]->GetNumberOfArrays(); x++)
        {
            if(fdVector[GK_FIELDDATASTR]->GetAbstractArray(x)->GetNumberOfComponents() == 3)
            {
                //vector we can transform
                this->AvailableVectorFields[GK_FIELDDATASTR].push_back(QString(fdVector[GK_FIELDDATASTR]->GetAbstractArray(x)->GetName()));

            }
            else if(fdVector[GK_FIELDDATASTR]->GetAbstractArray(x)->GetNumberOfComponents() == 1
                    && (fdVector[GK_FIELDDATASTR]->GetAbstractArray(x)->GetDataType() == VTK_DOUBLE
                        || fdVector[GK_FIELDDATASTR]->GetAbstractArray(x)->GetDataType() == VTK_FLOAT))
            {
                //component we can use for scalar
                this->AvailableScalarFields[GK_FIELDDATASTR].push_back(QString(fdVector[GK_FIELDDATASTR]->GetAbstractArray(x)->GetName()));

            }
        }
    }

    if(tdVector.size() > 0)
    {
        for(int x = 0; x < tdVector.size(); x++)
        {

            this->dataSources->AddArray(tdVector.keys()[x].toAscii().data());
            this->currentDataSourceList->InsertNextValue(tdVector.keys()[x].toAscii().data());

            //get number of columns
            QStringList tableKeys = tdVector.keys();
            QString currentTable = tableKeys[x];
            int numColumn = tdVector[currentTable]->GetNumberOfColumns();

            for(int y = 0; y < numColumn; y++)
            {
                if(tdVector[currentTable]->GetColumn(y)->GetNumberOfComponents() == 3)
                {
                    //vector we can transform
                    this->AvailableVectorFields[currentTable].push_back(QString(tdVector[currentTable]->GetColumn(y)->GetName()));

                }
                else if(tdVector[currentTable]->GetColumn(y)->GetNumberOfComponents() == 1
                        && (tdVector[currentTable]->GetColumn(y)->GetDataType() == VTK_DOUBLE
                            || tdVector[currentTable]->GetColumn(y)->GetDataType() == VTK_FLOAT))
                {
                    //component we can use for scalar
                    this->AvailableScalarFields[currentTable].push_back(QString(tdVector[currentTable]->GetColumn(y)->GetName()));

                }
            }

        }
    }


    return 1;
}


//===============================================//
vtkStringArray *gk_cxFormField::GetScalarFieldList()
{
    return this->currentScalarFieldsList;
}


//===============================================//
vtkStringArray *gk_cxFormField::GetVectorFieldList()
{
    this->generateVectorStringArray();
    return this->currentVectorFieldsList;
}

vtkStringArray *gk_cxFormField::GetTableArrayInfo()
{
    return this->GetVectorFieldList();
}


//===============================================//
void gk_cxFormField::VectorCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
    std::cerr << __FUNCTION__ << " has been called" << std::endl;

    gk_cxFormField* filter = static_cast<gk_cxFormField*>(clientdata);

    filter->currentVectorFieldsList->Reset();
    filter->generateVectorStringArray();
    filter->Modified();
}

//===============================================//
void gk_cxFormField::ScalarCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
    std::cerr << __FUNCTION__ << " has been called" << std::endl;

    gk_cxFormField* filter = static_cast<gk_cxFormField*>(clientdata);

    filter->currentScalarFieldsList->Reset();
    int numArrays = filter->scalarFields->GetNumberOfArrays();

    for(int x = 0; x < numArrays; x++)
    {
        filter->currentScalarFieldsList->InsertNextValue(filter->scalarFields->GetArrayName(x));
    }

    filter->Modified();
}

//===============================================//
void gk_cxFormField::SourceCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
    std::cerr << __FUNCTION__ << " has been called" << std::endl;

    gk_cxFormField* filter = static_cast<gk_cxFormField*>(clientdata);

    filter->currentDataSourceList->Reset();
    int numArrays = filter->dataSources->GetNumberOfArrays();

    for(int x = 0; x < numArrays; x++)
    {
        filter->currentDataSourceList->InsertNextValue(filter->dataSources->GetArrayName(x));
    }

    filter->Modified();


}

//===============================================//
void gk_cxFormField::AvailableSystemCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata)
{
    std::cerr << __FUNCTION__ << " has been called" << std::endl;

}

//===============================================//

