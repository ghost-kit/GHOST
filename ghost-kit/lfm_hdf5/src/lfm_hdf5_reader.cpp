#include "lfm_hdf5_reader.h"

#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredGrid.h"
#include "vtksys/SystemTools.hxx"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"
#include "vtkFloatArray.h"

#include "Io.hpp"

#include <iostream>

/*** Public Methods ***/

// Define New()
vtkStandardNewMacro(lfm_hdf5_reader);

//Removes all of the file names from actionalbe status
//  and resets all reader values to start
void lfm_hdf5_reader::RemoveAllFileNames()
{
    this->allFileNames.clear();
    this->resetAllData();
}



void lfm_hdf5_reader::AddFileName(const char *fileName)
{
    if(fileName)
    {
        this->allFileNames.append(QString(fileName));
    }
}

//Return the filename of the file actively parsed in memory
//  will return a NULL pointer if there is no parsed file
char *lfm_hdf5_reader::GetActiveFileName()
{
    if(this->activeFileName.count() > 0)
    {
        return this->activeFileName.toAscii().data();
    }
    else
    {
        return NULL;
    }
}

//Returns 1 if the request for information was successfull
//  0 otherwise
int lfm_hdf5_reader::RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{

    return 1;
}

//Returns 1 if the request for data was successfull
//  0 otherwise
int lfm_hdf5_reader::RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *)
{

    return 1;
}


//Returns the number of time steps active
int lfm_hdf5_reader::getNumberOfTimeSteps()
{
    return this->TimeStepValues.count();
}

//Returns a specific time step
//  returns -1 if the index does not exist
double lfm_hdf5_reader::getTimeStep(int index)
{
    if(this->TimeStepValues.count() > index)
    {
        return this->TimeStepValues[index];
    }
    else
    {
        return -1;
    }
}

void lfm_hdf5_reader::resetAllData()
{
    std::cout << "Resetting All Data" << std::endl;
}


/*** Protected Methods ***/

lfm_hdf5_reader::lfm_hdf5_reader()
{
    this->DebugOn();

    //setup everything
    this->resetAllData();

    //configure the initial state of the arrays
    this->PointDataArraySelection = vtkDataArraySelection::New();
    this->CellDataArraySelection  = vtkDataArraySelection::New();
    this->PointDataArraySelection->DisableAllArrays();
    this->CellDataArraySelection->DisableAllArrays();

    //Nothing has been processed yet, so dirty by default
    this->dirty = true;
}

lfm_hdf5_reader::~lfm_hdf5_reader()
{
    this->resetAllData();
    this->PointDataArraySelection->Delete();
    this->CellDataArraySelection->Delete();
}

/*** Private Methods ***/

