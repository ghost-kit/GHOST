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

vtkStandardNewMacro(enlilReader)


//---------------------------------------------------------------
//    Constructors and Destructors
//---------------------------------------------------------------
enlilReader::enlilReader()
{

    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;
    int nulExtent[6] = {0,0,0,0,0,0};

    //set the number of output ports you will need
    this->SetNumberOfOutputPorts(2);

    //set the number of input ports (Default 0)
    this->SetNumberOfInputPorts(0);

    //configure array status selectors
    this->PointDataArraySelection = vtkDataArraySelection::New();
    this->CellDataArraySelection  = vtkDataArraySelection::New();
    this->PointDataArraySelection->DisableAllArrays();
    this->CellDataArraySelection->DisableAllArrays();

    this->numberOfArrays = 0;

    //Configure sytem array interfaces
    this->Points = NULL;
    this->Radius = NULL;
    this->gridClean = false;

    this->SelectionObserver = vtkCallbackCommand::New();
    this->SelectionObserver->SetCallback(&enlilReader::SelectionCallback);
    this->SelectionObserver->SetClientData(this);
    this->PointDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);
    this->CellDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);




}

//---------------------------------------------------------------------------------------------
enlilReader::~enlilReader()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    this->PointDataArraySelection->Delete();
    this->CellDataArraySelection->Delete();
    this->SelectionObserver->Delete();


}



int enlilReader::findControlFile()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //TODO: find the control file in the current file directory.
    //  if cannot find it, deactivate useControlFile and throw
    //  A vtkErrorMacro()

    //    //Look in current directory for control file with the name of "control_file"
    //    QStringList directory = this->CurrentFile->getFileName().split("/");
    //    directory[directory.size()-1] = QString("control_file");

    //    QString CFPath = directory.join("/");

    //    //check existence
    //    std::ifstream file(CFPath.toAscii().data());
    //    if (file.good())
    //    {

    //        file.close();
    //        this->controlFileName.clear();
    //        this->controlFileName = CFPath;

    //        return true;
    //    }

    //    std::cerr << "File Not Found " << std::endl;

    //    file.close();
    //    this->controlFileName.clear();

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
int enlilReader::GetNumberOfPointArrays()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->PointDataArraySelection->GetNumberOfArrays();
}
//---------------------------------------------------------------------------------------------
/*
 * The Number of Cell Arrays in current selection
 *  This is an internal function
 */
int enlilReader::GetNumberOfCellArrays()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->CellDataArraySelection->GetNumberOfArrays();
}
//---------------------------------------------------------------------------------------------
/*
 *Return the NAME (characters) of the Point array at index
 *   This is an internal function
 */
const char* enlilReader::GetPointArrayName(int index)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->PointDataArraySelection->GetArrayName(index);
}

//---------------------------------------------------------------------------------------------
/*
 *Return the NAME (characters) of the Cell array at index
 *   This is an internal function
 */
const char* enlilReader::GetCellArrayName(int index)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->CellDataArraySelection->GetArrayName(index);
}

//---------------------------------------------------------------------------------------------
/*
 *Get the status of the Point Array of "name"
 *   This is an internal function
 */
int enlilReader::GetPointArrayStatus(const char *name)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->PointDataArraySelection->GetArraySetting(name);
}

//---------------------------------------------------------------------------------------------
/*
 *Get the status of the Cell Array of "name"
 *   This is an internal function
 */
int enlilReader::GetCellArrayStatus(const char *name)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->CellDataArraySelection->GetArraySetting(name);
}

//---------------------------------------------------------------------------------------------
/*
 *Set the status of the Point Array of "name"
 *   This is an internal function
 */
void enlilReader::SetPointArrayStatus(const char *name, int status)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //  std::cout << __FUNCTION__ << " Called with status: " << status << std::endl;

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
void enlilReader::SetCellArrayStatus(const char *name, int status)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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
void enlilReader::DisableAllPointArrays()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    this->PointDataArraySelection->DisableAllArrays();

    this->Modified();
}

//---------------------------------------------------------------------------------------------
/*
 *Disables ALL Cell arrays registered in system
 *   This is an internal function
 */
void enlilReader::DisableAllCellArrays()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    this->CellDataArraySelection->DisableAllArrays();

    this->Modified();
}

//---------------------------------------------------------------------------------------------
/*
 *Enables ALL Point arrays registered in system
 *   This is an internal function
 */
void enlilReader::EnableAllPointArrays()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    this->PointDataArraySelection->EnableAllArrays();

    this->Modified();
}

//---------------------------------------------------------------------------------------------
/*
 *Enables ALL Cell arrays registered in system
 *   This is an internal function
 */
void enlilReader::EnableAllCellArrays()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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

int enlilReader::CanReadFile(const char *filename)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //This doesn't really do anything right now...
    return 1;
}

//---------------------------------------------------------------------------------------------
int enlilReader::RequestInformation(
        vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector)
{

    return 1;
}


//---------------------------------------------------------------------------------------------
int enlilReader::RequestData(
        vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector)
{

    return 1;

}


//---------------------------------------------------------------------------------------------
//Methods for file series



void enlilReader::AddFileName(const char *fname)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //TODO: when adding file, must open briefly, get the time stamp, and close.
    double timestamp=0;

    enlil3DFile* file = new enlil3DFile(fname, "AU", 1.5e11);
    timestamp = file->getMJD();

    this->add3DFile(file,timestamp);
    this->Modified();

    std::cout << "Added File: " << fname << std::endl;

}



void enlilReader::RemoveAllFileNames()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //    this->fileNames.clear();
    //    this->numberOfArrays = 0;
    //    this->timesCalulated = false;
    //    this->NumberOfTimeSteps = 0;
    //    this->TimeSteps.clear();
    //    this->gridClean = false;
    //    this->_EvoFilesLoaded = false;
    //    this->_EvoFilesProcessed = false;
    //    this->clearEvoData();

    //    this->FileName = NULL;
    //    this->Modified();
}

unsigned int enlilReader::GetNumberOfFileNames()
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->_3dFiles.count();
}

//=================== END CORE METHODS =======================

//---------------------------------------------------------------------------------------------
//-- Callback
void enlilReader::SelectionCallback(
        vtkObject* object,
        unsigned long vtkNotUsed(eventid),
        void* clientdata,
        void* vtkNotUsed(calldata))
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;


    static_cast<enlilReader*>(clientdata)->Modified();
}


//---------------------------------------------------------------------------------------------
//------------------------------------------------------------
//    These methods to load the requested variables.
//  These are provided so that we can abstract out the reading
//  of the data from the rest of the reader.
//
//  Override these methods for your reader
//------------------------------------------------------------














//---------------------------------------------------------------------------------------------
//Status Check
int enlilReader::checkStatus(void *Object, char *name)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    if(Object == NULL)
    {
        std::cerr << "ERROR: " << name
                  << " has failed to initialize"
                  << std::endl;

        return 0;
    }

    return 1;
}


//---------------------------------------------------------------------------------------------
//get the dimensions from the extents provided
void enlilReader::extractDimensions(int dims[], int extent[])
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    dims[0] = extent[1] - extent[0]+1;
    dims[1] = extent[3] - extent[2]+1;
    dims[2] = extent[5] - extent[4]+1;
}





void enlilReader::add3DFile(enlil3DFile *file, double mjd)
{
    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    if(_3dFiles.contains(mjd))
    {
        delete _3dFiles[mjd];
    }
    this->_3dFiles[mjd] = file;
}



