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

    //    //configure array status selectors
    this->PointDataArraySelection = vtkDataArraySelection::New();
    this->CellDataArraySelection  = vtkDataArraySelection::New();
    this->PointDataArraySelection->DisableAllArrays();
    this->CellDataArraySelection->DisableAllArrays();

    this->numberOfArrays = 0;

    this->__grid = NULL;
    this->__gridClean = false;

    this->SelectionObserver = vtkCallbackCommand::New();
    this->SelectionObserver->SetCallback(&vtkEnlilReader::SelectionCallback);
    this->SelectionObserver->SetClientData(this);
    this->PointDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);
    this->CellDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);


}

//---------------------------------------------------------------------------------------------
vtkEnlilReader::~vtkEnlilReader()
{

    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    this->PointDataArraySelection->Delete();
    this->CellDataArraySelection->Delete();
    this->SelectionObserver->Delete();

    if(this->__grid) this->__grid->Delete();

}



int vtkEnlilReader::findControlFile()
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;



    return false;
}

/**
 * @brief vtkEnlilReader::getWholeExtent
 * @return
 */
QVector<enlilExtent> vtkEnlilReader::getWholeExtent()
{
    QMap<QString, enlilExtent>  extentMap = this->_3Dfiles[current_MJD]->getWholeExtents();
    QVector<enlilExtent> wholeExtents;
    wholeExtents.push_back(extentMap["n1"]);
    wholeExtents.push_back(extentMap["n2"]);
    wholeExtents.push_back(extentMap["n3"]);

    return wholeExtents;

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
    std::cerr << "Starting " << __FUNCTION__ << std::flush << std::endl;
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
        double timeRange[2];
        QPair<double, double> minmax = this->getTimeStepRange();

        timeRange[0] = minmax.first;
        timeRange[1] = minmax.second;

        //get the extents from the file
        int WholeExtent[6];
        QVector<enlilExtent> extents = this->getWholeExtent();
        WholeExtent[0] = extents[0].first;
        WholeExtent[1] = extents[0].second;
        WholeExtent[2] = extents[1].first;
        WholeExtent[3] = extents[1].second;
        WholeExtent[4] = extents[2].first;
        WholeExtent[5] = extents[2].second;

        //set the time step data itself
        DataOutputInfo->Set(
                    vtkStreamingDemandDrivenPipeline::TIME_STEPS(),
                    this->_3Dfiles.keys().toVector().toStdVector().data(),
                    this->_3Dfiles.count());

        //set the start and end times
        DataOutputInfo->Set(
                    vtkStreamingDemandDrivenPipeline::TIME_RANGE(),
                    timeRange,
                    2);

        //provide Paraview with the range of extents in this file
        DataOutputInfo->Set(
                    vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
                    WholeExtent,
                    6);


    }


    //Set Port 1 (EVO Data) to be one time step
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
    this->SetProgressText("Loading Data...");
    double progress = 0.0;

    this->SetProgress(progress);

    //get correct time postions
    this->current_MJD = this->getRequestedTime(outputVector);
    progress += 0.1;
    this->SetProgress(progress);

    //Prep the output
    int newExtent[6];

    std::cout << "Getting Data" << std::flush << std::endl;

    vtkStructuredGrid* Data = vtkStructuredGrid::GetData(outputVector, 0);
    vtkInformation* fieldInfo = outputVector->GetInformationObject(0);

    int status = this->checkStatus(Data, (char*)"Data Array Structured Grid");

    if(status)
    {
        std::cout << "Getting Requested Extents" << std::flush << std::endl;
        //get the requested extent from ParaView
        fieldInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), newExtent);

        //check to see if the extents have changed

        if(!__eq(newExtent, this->__getCurrentExtents()))
        {
            this->__gridClean = false;
        }


        //set current file extent
        this->_3Dfiles[current_MJD]->setSubExtents(newExtent);

        //acknowledge paraview on extents
        Data->SetExtent(newExtent);

        //set grid
        Data->SetPoints(this->getGrid());

        progress += 0.4;
        this->SetProgress(progress);

        //set the data requested
        int totalNumberArrays = this->PointDataArraySelection->GetNumberOfArrays();

        for(int arrayNum=0; arrayNum < totalNumberArrays; arrayNum++)
        {
            QString Name(this->PointDataArraySelection->GetArrayName(arrayNum));
            std::cout << "Populating Data " << qPrintable(Name) << std::flush << std::endl;

            QStringList Var = this->__ArrayNameMap[Name];
            int numBlocks = this->_3Dfiles[current_MJD]->getNumberOfBlocks(qPrintable(Var[0]));

            for(int block=0; block < numBlocks; block++)
            {
                if(this->PointDataArraySelection->ArrayIsEnabled(qPrintable(Name)))
                {
                    vtkFloatArray* data = this->getDataFromFile(Name, block);
                    Data->GetPointData()->AddArray(data);
                    data->Delete();
                }

            }
            progress += (.5/totalNumberArrays);
            this->SetProgress(progress);
        }

    }


    this->SetProgress(1.0);

    return 1;

}

//---------------------------------------------------------------------------------------------
//Get the Requested Time Step
double vtkEnlilReader::getRequestedTime(vtkInformationVector* outputVector)
{
    std::cout << __FUNCTION__ << " Start" << std::endl << std::flush;

    vtkInformation* outInfo = outputVector->GetInformationObject(0);

    double requestedTimeValue = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());

    std::cout << "requested time: " << requestedTimeValue << std::endl;

    if(requestedTimeValue == 0)
    {
        //single time step
        requestedTimeValue = this->_3Dfiles.keys()[0];
    }

    std::cout << "Returning from " << __FUNCTION__ << std::endl;

    return requestedTimeValue;
}

/**
* @brief vtkEnlilReader::buildGrid
* @return
*/
vtkPoints *vtkEnlilReader::buildGrid()
{
    //FIXME: Memory Managment - look for best way to deal with this
    vtkPoints *grid = vtkPoints::New(VTK_FLOAT);

    this->_3Dfiles[current_MJD]->setGridSpacingType(ENLIL_GRIDSPACING_CT);

    QVector<QVector<double> > gridSpace = this->_3Dfiles[current_MJD]->getGridSpacing();

    int loop=0;
    int loopMax= gridSpace.count();

    for(loop =0; loop < loopMax; loop++)
    {
        grid->InsertNextPoint(gridSpace[loop][0], gridSpace[loop][1], gridSpace[loop][2]);
    }

    return grid;

}

/**
 * @brief vtkEnlilReader::getGrid
 * @return
 */
vtkPoints *vtkEnlilReader::getGrid()
{
    //FIXME: Find out why grid is changing scale with switched.  Check 3Dfile class.
    if(!this->__gridClean || !this->__grid)
    {
        if(this->__grid) this->__grid->Delete();

        this->__grid = this->buildGrid();
    }

    //mark the grid as clean
    this->__gridClean = true;

    return this->__grid;
}

/**
 * @brief vtkEnlilReader::__getCurrentExtents
 * @return
 */
int *vtkEnlilReader::__getCurrentExtents()
{
    QMap<QString, enlilExtent> extents = this->_3Dfiles[this->current_MJD]->getCurrentExtents();

    this->__currentExtents[0] = extents["n1"].first;
    this->__currentExtents[1] = extents["n1"].second;
    this->__currentExtents[2] = extents["n2"].first;
    this->__currentExtents[3] = extents["n2"].second;
    this->__currentExtents[4] = extents["n3"].first;
    this->__currentExtents[5] = extents["n3"].second;

    return this->__currentExtents;
}

/**
 * @brief vtkEnlilReader::getDataFromFile
 * @return
 */
vtkFloatArray *vtkEnlilReader::getDataFromFile(QString arrayName, int block)
{

    QStringList vars = this->__ArrayNameMap[arrayName];
    //setup new array
    vtkFloatArray *newArray = vtkFloatArray::New();
    QString newArrayName = arrayName;



    newArrayName += (" " + QString::number(block));


    newArray->SetName(qPrintable(newArrayName));

    //check to see if it is a vector or scalar
    if(vars.count() > 1)
    {
        //Vector
        newArray->SetNumberOfComponents(3);
        QVector<QVector<float> > data;
        data = this->_3Dfiles[this->current_MJD]->asFloat(qPrintable(vars[0]), qPrintable(vars[1]), qPrintable(vars[2]), block);
        int dataSize = data.count();
        for(int x = 0; x < dataSize; x++)
        {
            newArray->InsertNextTuple3(data[x][0], data[x][1], data[x][2]);
        }

    }
    else
    {
        //Scalar
        newArray->SetNumberOfComponents(1);
        QVector<float> data;
        data = this->_3Dfiles[this->current_MJD]->asFloat(qPrintable(vars[0]), block);
        int dataSize = data.count();
        for(int x = 0; x < dataSize; x++)
        {
            newArray->InsertNextValue(data[x]);
        }
    }

    //return the VTK array
    return newArray;
}


//---------------------------------------------------------------------------------------------
//Methods for file series

void vtkEnlilReader::AddFileName(const char *fname)
{
    /* Updated for Enlil Reader 3.0 */


    enlil3DFile* newFile = new enlil3DFile(QString(fname),  GRID_SCALE::ScaleUnits[GRID_SCALE::AU], GRID_SCALE::ScaleFactor[GRID_SCALE::AU]);
    double mjd = newFile->getMJD();

    this->_3Dfiles[mjd] = newFile;

//    std::cerr << "Added FileName for timestep: " << mjd << std::endl;

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
    //TODO: Need to have a map of these names to variable.  Maybe a reverse lookup within enlil3dfile?

    QStringList scalars = this->_3Dfiles[this->current_MJD]->getScalarList();
    QStringList vectors = this->_3Dfiles[this->current_MJD]->getVectorList();

    int numScalars = scalars.count();
    int numVectors = vectors.count();

    for(int x=0; x < numScalars; x++)
    {
        if(!this->_3Dfiles[this->current_MJD]->isSingularity(scalars[x]))
        {

            //get the long name
            QString lname = this->_3Dfiles[this->current_MJD]->getVarLongName(qPrintable(scalars[x]));
            QStringList llist;

            //store the variable name to the map

            llist.push_back(scalars[x]);

            this->__ArrayNameMap[lname] = llist;

            //add the name to the list
            this->PointDataArraySelection->AddArray(qPrintable(lname));

        }

    }

    for(int x=0; x < numVectors; x++)
    {
        QString longName = this->_3Dfiles[this->current_MJD]->getVarLongName(qPrintable(vectors[x]+ QString("1"))) ;
        QStringList llist;

        //create the names of the indivual components of the vector
        llist.push_back(vectors[x]+"1");
        llist.push_back(vectors[x]+"2");
        llist.push_back(vectors[x]+"3");

        //Fix the long name to make it pretty
        QStringList nameParts = longName.split("-");
        if(nameParts.count() >= 2)
        {
            longName = nameParts[1];
            longName[0] = longName[0].toTitleCase();
        }

        //record what variable the names belongs to
        this->__ArrayNameMap[longName] = llist;

        //Add the name to the list
        this->PointDataArraySelection->AddArray(qPrintable(longName));


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

/**
 * @brief vtkEnlilReader::getTimeStepRange
 * @return
 */
QPair<double, double> vtkEnlilReader::getTimeStepRange()
{
    QList<double> keys = this->_3Dfiles.keys();

    QPair<double, double> minMax;

    minMax.first = keys.first();
    minMax.second = keys.last();

    return minMax;
}


bool vtkEnlilReader::__eq(int extent1[], int extent2[])
{
    return(extent1[0] == extent2[0] && extent1[1] == extent2[1]
            && extent1[2] == extent2[2] && extent1[3] == extent2[3]
            && extent1[4] == extent2[4] && extent1[5] == extent2[5]);
}

