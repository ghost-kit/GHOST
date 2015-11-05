#ifndef vtkEnlilReader_H
#define vtkEnlilReader_H

#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkStructuredGrid.h"
#include "vtkTable.h"
#include "vtkType.h"
#include "vtkDataArraySelection.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkCallbackCommand.h"

#include "enlilControlFile.h"
#include "enlil3dfile.h"
#include "enlilEvoFile.h"

class  enlilReader : public vtkMultiBlockDataSetAlgorithm
{

public:
    static enlilReader* New();

    vtkTypeMacro(enlilReader, vtkMultiBlockDataSetAlgorithm)
    void PrintSelf(ostream &os, vtkIndent indent);

    void SetGridScaleType(int value)
    {
        this->GridScaleType = value;
        this->gridClean = false;
        this->Modified();
    }

    vtkGetMacro(GridScaleType, int)


    void SetDataUnits(int _arg)
    {

        this->Modified();
    }

    char* GetCurrentFileName();

    vtkGetMacro(DataUnits, int)

    vtkSetVector6Macro(WholeExtent, int)
    vtkGetVector6Macro(WholeExtent, int)

    vtkSetVector6Macro(SubExtent, int)
    vtkGetVector6Macro(SubExtent, int)

    void setUseControlFile(int status);

    // Description:
    // The following methods allow selective reading of solutions fields.
    int GetNumberOfPointArrays();
    int GetNumberOfCellArrays();

    const char* GetPointArrayName(int index);
    const char* GetCellArrayName(int index);

    int  GetPointArrayStatus(const char* name);
    int  GetCellArrayStatus(const char* name);

    void SetPointArrayStatus(const char* name, int status);
    void SetCellArrayStatus(const char* name, int status);

    void DisableAllPointArrays();
    void DisableAllCellArrays();

    void EnableAllPointArrays();
    void EnableAllCellArrays();

    void AddFileName(const char* fname);
    void RemoveAllFileNames();
    unsigned int GetNumberOfFileNames();
    const char *GetFileName(unsigned int idx);

protected:

    enlilReader();
    ~enlilReader();

    // Required Paraview Functions
    static int CanReadFile(const char* filename);

    virtual int RequestData(
            vtkInformation* request,
            vtkInformationVector** inputVector,
            vtkInformationVector* outputVector);

    virtual int RequestInformation(
            vtkInformation* request,
            vtkInformationVector** inputVector,
            vtkInformationVector* outputVector);

    static void SelectionCallback(
            vtkObject *caller,
            unsigned long eid,
            void *clientdata,
            void *calldata);

    virtual int FillOutputPortInformation(int, vtkInformation*);


private:    //data
    // Selected field of interest
    vtkDataArraySelection* PointDataArraySelection;
    vtkDataArraySelection* CellDataArraySelection;

    // Observer to modify this object when array selections are modified
    vtkCallbackCommand* SelectionObserver;

    //Data interface information
    vtkSmartPointer<vtkPoints> Points;       // Structured grid geometry
    vtkSmartPointer<vtkFloatArray> Radius;   // Radius Grid Data

    QStringList _3DfileNames;
    QStringList _EVOfileNames;
    QString     _controlFileName;

    // Time step information
    int NumberOfTimeSteps;                 // Number of time steps

    // Extent information
    vtkIdType NumberOfTuples;  // Number of tuples in subextent

    // Field
    int WholeExtent[6];       // Extents of entire grid
    int SubExtent[6];         // Processor grid extent
    int UpdateExtent[6];
    int Dimension[3];         // Size of entire grid
    int SubDimension[3];      // Size of processor grid


    int GridScaleType;
    int DataUnits;
    bool gridClean;
    int numberOfArrays;
    bool useControlFile;

    //EVO Data
    QMap<QString, enlilEvoFile*> evoFiles;   //environment files mapped by environment

    //3D Data
    QMap<double, enlil3DFile*> _3dFiles;    //3D Files mapped by mjd

    //Control File
    enlilControlFile *_controlFile;          //CCMC Control File


private:    //methods
    // Request Information Helpers
    double getRequestedTime(vtkInformationVector *outputVector);
    int calculateTimeSteps();
    int checkStatus(void* Object, char* name);

    void addPointArray(char* name);
    void addPointArray(char* name1, char* name2, char* name3);
    void extractDimensions(int dims[], int extent[]);

    //control file
    int findControlFile();

    //3D Data Files
    void add3DFile(enlil3DFile *file, double mjd);
    void load3DData(vtkInformationVector *&outputVector);
    void clear3DData();

    enlilReader(const enlilReader&);  // Not implemented.
    void operator=(const enlilReader&);  // Not implemented.
};



#endif // vtkEnlilReader_H
