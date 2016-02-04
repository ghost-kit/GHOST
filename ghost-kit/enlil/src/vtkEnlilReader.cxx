#include "vtkEnlilReader.h"

#include "vtkCallbackCommand.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkCellType.h"
#include "vtkDataArraySelection.h"
#include "vtkFloatArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkAbstractArray.h"
#include "vtkTable.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkVariantArray.h"
#include "vtkObjectBase.h"
#include "vtkStringArray.h"

#include "vtkStringArray.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkPoints.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtksys/SystemTools.hxx"
#include "vtkType.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "vtkMultiProcessController.h"
#include "vtkToolkits.h"

#include <vtknetcdf/include/netcdf.h>
#include <vtknetcdf/cxx/netcdfcpp.h>
#include <iostream>

#include "enlilEvoFile.h"
#include "vtkMultiBlockDataSet.h"

#include "ltrDateTime.h"
#include "readerCache.h"
#include "vtkNew.h"
#include <QString>
#include <QStringList>

#include <QDir>

vtkStandardNewMacro(vtkEnlilReader)


//---------------------------------------------------------------
//    Constructors and Destructors
//---------------------------------------------------------------
vtkEnlilReader::vtkEnlilReader()
{
    this->DebugOn();
    this->FileName = NULL;

    //set the number of output ports you will need
    this->SetNumberOfOutputPorts(2);

    //set the number of input ports (Default 0)
    this->SetNumberOfInputPorts(0);

    std::cout << "Ports Set" << std::endl << std::flush;

//    //configure array status selectors
    this->PointDataArraySelection = vtkDataArraySelection::New();
    this->CellDataArraySelection  = vtkDataArraySelection::New();
    this->PointDataArraySelection->DisableAllArrays();
    this->CellDataArraySelection->DisableAllArrays();

    this->numberOfArrays = 0;

    std::cout << "Arrays Set" << std::endl << std::flush;

    this->SelectionObserver = vtkCallbackCommand::New();
    this->SelectionObserver->SetCallback(&vtkEnlilReader::SelectionCallback);
    this->SelectionObserver->SetClientData(this);
    this->PointDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);
    this->CellDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);

    std::cout << "Callbacks Set" << std::endl << std::flush;


}

//---------------------------------------------------------------------------------------------
vtkEnlilReader::~vtkEnlilReader()
{

    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    this->PointDataArraySelection->Delete();
    this->CellDataArraySelection->Delete();
    this->SelectionObserver->Delete();

}



int vtkEnlilReader::findControlFile()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;



    return false;
}

//---------------------------------------------------------------------------------------------
//-------------------------------------------------------------
//    The following Methods provide basic functionality for
//  Selective read arrays.  These methods provide the list of
//  arrays in Paraview.  You MUST populate the arrays in you
//  RequestInformation routine.  This can be done with the
//  *****DataArraySlection->AddArray(char* name) routines
//  where ***** represents the type of arrays you are
//  populating. (i.e. Point or Cell)
//-------------------------------------------------------------

/*
 * The Number of Point Arrays in current selection
 *  This is an internal function
 */
int vtkEnlilReader::GetNumberOfPointArrays()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    return this->PointDataArraySelection->GetNumberOfArrays();
}
//---------------------------------------------------------------------------------------------
/*
 * The Number of Cell Arrays in current selection
 *  This is an internal function
 */
int vtkEnlilReader::GetNumberOfCellArrays()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    return this->CellDataArraySelection->GetNumberOfArrays();
}
//---------------------------------------------------------------------------------------------
/*
 *Return the NAME (characters) of the Point array at index
 *   This is an internal function
 */
const char* vtkEnlilReader::GetPointArrayName(int index)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    return this->PointDataArraySelection->GetArrayName(index);
}

//---------------------------------------------------------------------------------------------
/*
 *Return the NAME (characters) of the Cell array at index
 *   This is an internal function
 */
const char* vtkEnlilReader::GetCellArrayName(int index)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    return this->CellDataArraySelection->GetArrayName(index);
}

//---------------------------------------------------------------------------------------------
/*
 *Get the status of the Point Array of "name"
 *   This is an internal function
 */
int vtkEnlilReader::GetPointArrayStatus(const char *name)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    return this->PointDataArraySelection->GetArraySetting(name);
}

//---------------------------------------------------------------------------------------------
/*
 *Get the status of the Cell Array of "name"
 *   This is an internal function
 */
int vtkEnlilReader::GetCellArrayStatus(const char *name)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    return this->CellDataArraySelection->GetArraySetting(name);
}

//---------------------------------------------------------------------------------------------
/*
 *Set the status of the Point Array of "name"
 *   This is an internal function
 */
void vtkEnlilReader::SetPointArrayStatus(const char *name, int status)
{
      std::cout << __FUNCTION__ << " Called with status: " << status << std::endl << std::flush;

    if(status)
    {
        //        std::cout << "Enabling " << name << std::endl;
        this->PointDataArraySelection->EnableArray(name);
    }
    else
    {
        //        std::cout << "Disabling " << name << std::endl;

        this->PointDataArraySelection->DisableArray(name);
    }

    this->Modified();

    //    std::cout /*<< __FILE__ << " " << __LINE__ << " " <<  __FUNCTION__*/
    //            << " Status of Array " << name << ": "
    //            << this->PointDataArraySelection->ArrayIsEnabled(name) << std::endl;

}

//---------------------------------------------------------------------------------------------
/*
 *Set the status of the Cell Array of "name"
 *   This is an internal function
 */
void vtkEnlilReader::SetCellArrayStatus(const char *name, int status)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    if(status == 1)
        this->CellDataArraySelection->EnableArray(name);
    else
        this->CellDataArraySelection->DisableArray(name);

    //  std::cout << __FUNCTION__ << " Called with status: " << status << std::endl;


    this->Modified();

}

//---------------------------------------------------------------------------------------------
/*
 *Disables ALL Point arrays registered in system
 *   This is an internal function
 */
void vtkEnlilReader::DisableAllPointArrays()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    this->PointDataArraySelection->DisableAllArrays();

    this->Modified();
}

//---------------------------------------------------------------------------------------------
/*
 *Disables ALL Cell arrays registered in system
 *   This is an internal function
 */
void vtkEnlilReader::DisableAllCellArrays()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    this->CellDataArraySelection->DisableAllArrays();

    this->Modified();
}

//---------------------------------------------------------------------------------------------
/*
 *Enables ALL Point arrays registered in system
 *   This is an internal function
 */
void vtkEnlilReader::EnableAllPointArrays()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    this->PointDataArraySelection->EnableAllArrays();

    this->Modified();
}

//---------------------------------------------------------------------------------------------
/*
 *Enables ALL Cell arrays registered in system
 *   This is an internal function
 */
void vtkEnlilReader::EnableAllCellArrays()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    this->CellDataArraySelection->EnableAllArrays();
    this->Modified();
}
//=============== END SELECTIVE READER METHODS================

//---------------------------------------------------------------------------------------------
//------------------------------------------------------------
//    These functions are the meat of the readers... i.e. they
//  are the calls that ParaView uses to get information from
//  your data source.   This is where the logic of the reader
//  is implemented.
//------------------------------------------------------------

int vtkEnlilReader::CanReadFile(const char *filename)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;
    std::cout << "Checking File Readability" << std::endl << std::flush;

    //This doesn't really do anything right now...
    return 1;
}

//---------------------------------------------------------------------------------------------
int vtkEnlilReader::RequestInformation(
        vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    std::cerr << "Starting Request Information" << std::flush << std::endl;
    int status = 0;

    // Array names and extents
    vtkInformation* DataOutputInfo = outputVector->GetInformationObject(0);
    status = this->checkStatus(
                DataOutputInfo,
                (char*)" Array Name: Data Info Output Information");


    //get information from the first file in the list... it has to come from somewhere...

    if(status)
    {

        //set current time step
        if(this->_3Dfiles.count() > 0)
        {
            this->current_MJD = this->_3Dfiles.keys()[0];
        }
        else
        {
            std::cerr << "INVALID TIME STEP" << std::endl;
            return 0;
        }

        std::cerr << "getting array names" << std::flush << std::endl;
        //Work Around for restore state problems
        if(this->numberOfArrays == 0)
        {

            //Set the Names of the Arrays
            this->__PopulateArrays();

        }

        /*Set Information*/
        //Set Time

        DataOutputInfo->Set(
                    vtkStreamingDemandDrivenPipeline::TIME_STEPS(),
                    this->TimeSteps.data(),
                    this->NumberOfTimeSteps);

        DataOutputInfo->Set(
                    vtkStreamingDemandDrivenPipeline::TIME_RANGE(),
                    this->timeRange,
                    2);



        //Set Extents
        DataOutputInfo->Set(
                    vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
                    this->WholeExtent,
                    6);


    }


    //we need port 1 to be only one time step.
    DataOutputInfo = outputVector->GetInformationObject(1);
    status = this->checkStatus(
                DataOutputInfo,
                (char*)" Array Name: Data Info Output Information");

    if(status)
    {

        DataOutputInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
        DataOutputInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());


    }

    std::cerr << "Request Information Completed Successfully" << std::flush << std::endl;

    return 1;
}


//---------------------------------------------------------------------------------------------
int vtkEnlilReader::RequestData(
        vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector)
{
        std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;



    return 1;

}

//---------------------------------------------------------------------------------------------
//Get the Requested Time Step
double vtkEnlilReader::getRequestedTime(vtkInformationVector* outputVector)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;


    return 0;
}


//---------------------------------------------------------------------------------------------
//Methods for file series

void vtkEnlilReader::AddFileName(const char *fname)
{
    /* Updated for Enlil Reader 3.0 */


    enlil3DFile* newFile = new enlil3DFile(QString(fname), "m", 1);
    double mjd = newFile->getMJD();

    this->_3Dfiles[mjd] = newFile;

    std::cerr << "Added FileName for timestep: " << mjd << std::endl;

    this->Modified();
}

const char* vtkEnlilReader::GetFileName(unsigned int idx)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    if(this->fileNames.count() > idx)
    {
        return this->fileNames[idx].toAscii().data();
    }
    else
    {
        std::cerr << "Invalid File Number Requested" << std::endl << std::flush;
    }
}

const char *vtkEnlilReader::GetCurrentFileName()
{
    if(this->_3Dfiles.contains(current_MJD))
    {
        return this->_3Dfiles[current_MJD]->getName().toAscii().data();
    }
    else
    {
        std::cerr << "Invalid Time Step Reference" << std::endl << std::flush;
    }
}

void vtkEnlilReader::RemoveAllFileNames()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    this->fileNames.clear();
//    this->numberOfArrays = 0;
//    this->timesCalulated = false;
//    this->NumberOfTimeSteps = 0;
//    this->TimeSteps.clear();
//    this->gridClean = false;
//    this->EvoFilesLoaded = false;
//    this->EvoFilesProcessed = false;
//    this->clearEvoData();

    this->FileName = NULL;
    this->Modified();
}

unsigned int vtkEnlilReader::GetNumberOfFileNames()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    return this->fileNames.size();
}

//=================== END CORE METHODS =======================

//---------------------------------------------------------------------------------------------
//-- Callback
void vtkEnlilReader::SelectionCallback(
        vtkObject* object,
        unsigned long vtkNotUsed(eventid),
        void* clientdata,
        void* vtkNotUsed(calldata))
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    static_cast<vtkEnlilReader*>(clientdata)->Modified();
}


//---------------------------------------------------------------------------------------------






//---------------------------------------------------------------------------------------------
//--------------------------------------------------------------
//    Output Port Configuration
//--------------------------------------------------------------
int vtkEnlilReader::FillOutputPortInformation(int port, vtkInformation* info)
{

    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    if (port==0)
    {
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredGrid");
    }

    if(port==1)
    {
        //        std::cout << "EVO Files (Multi-block)" << std::endl;
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkMultiBlockDataSet");
    }

    return this->Superclass::FillInputPortInformation(port, info);;
}

void vtkEnlilReader::__PopulateArrays()
{
    QStringList scalars = this->_3Dfiles[this->current_MJD]->getScalarList();
    QStringList vectors = this->_3Dfiles[this->current_MJD]->getVectorList();

    int numScalars = scalars.count();
    int numVectors = vectors.count();

    for(int x=0; x < numScalars; x++)
    {
        if(!this->_3Dfiles[this->current_MJD]->isSingularity(scalars[x]))
        {
            QString lname = this->_3Dfiles[this->current_MJD]->getVarLongName(qPrintable(scalars[x]));
            QStringList llist;
            llist.push_back(scalars[x]);

            //add the name to the list
            this->PointDataArraySelection->AddArray(qPrintable(lname));

            //record what variable the name belongs to
            this->ArrayNameMap[lname] = llist;
        }
    }

    for(int x=0; x < numVectors; x++)
    {
        QString longName = this->_3Dfiles[this->current_MJD]->getVarLongName(qPrintable(vectors[x]+ QString("1"))) ;
        QStringList llist;
        llist.push_back(vectors[x]+"1");
        llist.push_back(vectors[x]+"2");
        llist.push_back(vectors[x]+"3");

        QStringList nameParts = longName.split("-");
        if(nameParts.count() >= 2)
        {
            longName = nameParts[1];
            longName[0] = longName[0].toTitleCase();
        }

        //Add the name to the list
        this->PointDataArraySelection->AddArray(qPrintable(longName));

        //record what variable the names belongs to
        this->ArrayNameMap[longName] = llist;
    }
}

void vtkEnlilReader::locateAndLoadEvoFiles()
{

}

//---------------------------------------------------------------------------------------------

//================= END PORT CONFIGURATION ===================

//------------------------------------------------------------
//    Internal functions -- required for system to work
//------------------------------------------------------------
void vtkEnlilReader::PrintSelf(ostream &os, vtkIndent indent)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    this->Superclass::PrintSelf(os, indent);
}

void vtkEnlilReader::updateControlFile(int status)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    if(status)
    {
        if(this->findControlFile())
        {
            //control file found
            if(this->controlFile) delete this->controlFile;
            this->controlFile = new enlilControlFile(this->controlFileName.toAscii().data());
            this->useControlFile = 1;
        }
        else
        {
            std::cerr << "Control file NOT located" << std::endl << std::flush;
            //control file not found
            if(this->controlFile) delete this->controlFile;
            this->controlFile = NULL;
            this->controlFileName.clear();
            this->useControlFile = 0;
        }

    }
    else
    {
        //remove control file
        if(this->controlFile) delete this->controlFile;
        this->controlFile = NULL;
        this->controlFileName.clear();
        this->useControlFile = 0;
    }
}

void vtkEnlilReader::setUseControlFile(int status)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

//    this->useControlFile = status;
//    if(!status)
//    {
//        //this will deactivate the control file
//        this->updateControlFile(false);

//    }

//    this->Modified();
}

//---------------------------------------------------------------------------------------------
//Status Check
int vtkEnlilReader::checkStatus(void *Object, char *name)
{
    if(Object == NULL)
    {
        std::cerr << "ERROR: " << name
                  << " has failed to initialize"
                  << std::endl;

        return 0;
    }

    return 1;
}

