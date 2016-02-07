#ifndef vtkEnlilReader_H
#define vtkEnlilReader_H


#include "vtkStructuredGridAlgorithm.h"
#include "vtkIOParallelNetCDFModule.h" // For export macro
#include "vtkSmartPointer.h"
#include<map>
#include<string>
#include<vector>
#include<QString>
#include<QStringList>

#include "enlilControlFile.h"
#include "enlilEvoFile.h"
#include "enlil3dfile.h"
#include "enlil3Dvar.h"
#include "enlilAtt.h"


class vtkDataArraySelection;
class vtkCallbackCommand;
class vtkMultibockProcessController;
class vtkStringArray;
class vtkFloatArray;
class vtkFloatArray;
class vtkIntArray;
class vtkPoints;
class vtkTable;
class vtkStructuredGrid;
class vtkStructuredGridAlgorithm;


namespace GRID_SCALE
{
enum ScaleType{
    NONE   = 0,
    REARTH = 1,
    RSOLAR = 2,
    AU     = 3    
};

static const float ScaleFactor[4] = { 1.0,
                                      6.5e6,
                                      6.955e8,
                                      1.5e11 };

static const char* ScaleUnits[4] = {"m",
                                    "Re",
                                    "Rs",
                                    "AU"};



}

namespace UNITS
{
static const double emu = 1.67262158e-27;
static const double km2cm = 1e6;
static const double km2m = 1e3;
}

namespace DATA_TYPE
{
enum dataType{
    PDENSITY = 0,
    CDENSITY = 1,
    TEMP = 2,
    POLARITY = 3,
    BFIELD = 4,
    VELOCITY = 5,
    RVELOCITY = 6
};


}


/** READER PRIME **/
class VTKIOPARALLELNETCDF_EXPORT vtkEnlilReader : public vtkStructuredGridAlgorithm
{

public:
    static vtkEnlilReader* New();

    vtkTypeMacro(vtkEnlilReader, vtkStructuredGridAlgorithm)
    void PrintSelf(ostream &os, vtkIndent indent);

    // Set/get macros
    void SetGridScaleType(int value)
    {
        this->GridScaleType = value;

        int fileCount = this->_3Dfiles.count();
        QList<double> fileKeys = this->_3Dfiles.keys();

        for (int file=0; file < fileCount; file++)
        {
            this->_3Dfiles[fileKeys[file]]->setScale_factor(QString(GRID_SCALE::ScaleUnits[this->GridScaleType]), GRID_SCALE::ScaleFactor[this->GridScaleType]);
        }

        this->__gridClean = false;
        this->Modified();
    }

    vtkGetMacro(GridScaleType, int)


    void SetDataUnits(int _arg)
    {

        std::cout << "Updating Units: " << std::endl;
        this->DataUnits = _arg;

        this->__gridClean=false;
        this->Modified();
    }

    vtkGetMacro(DataUnits, int)




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

    const char *GetCurrentFileName();


    void readVector(std::string array, vtkFloatArray *DataArray, vtkInformationVector* outputVector, const int &dataID);
    void readScalar(vtkStructuredGrid *Data, vtkFloatArray *DataArray, std::string array, vtkInformationVector* outputVector, int dataID);
    void getDataID(std::string array, int &dataID);
    void updateControlFile(int status);
protected:

    vtkEnlilReader();
    ~vtkEnlilReader();

    char* FileName;            // Base file name
    int GridScaleType;
    int DataUnits;
    int numberOfArrays;
    bool useControlFile;

    //control file
    enlilControlFile *controlFile;
    QString controlFileName;
    int findControlFile();

    // Extent information
    vtkIdType NumberOfTuples;  // Number of tuples in subextent

    // Field
    QVector<enlilExtent> getWholeExtent();

    // Check to see if info is clean
    bool infoClean;

    //FileNames
    QStringList fileNames;

    //this map holds the positions of artifacts based on time step
    std::map< double, std::map<std::string, std::vector<double> > > positions;

    char* CurrentFileName;
    void SetCurrentFileName(const char* fname);

    // Selected field of interest
    vtkDataArraySelection* PointDataArraySelection;
    vtkDataArraySelection* CellDataArraySelection;

    // Observer to modify this object when array selections are modified
    vtkCallbackCommand* SelectionObserver;

    int getSerialNumber()
    {
        static int number = 0;
        return ++number;
    }

    inline void clearString(char* string, int size)
    {
        for(int x = 0; x < size; x++)
        {
            string[x] = '\0';
        }
    }


    // Request Information Helpers
    double getRequestedTime(vtkInformationVector *outputVector);
    int PopulateArrays();


    // Required Paraview Functions
    virtual int CanReadFile(const char* filename);

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


private:
    //Data manipulators
    void __PopulateArrays();                    //method to populate names from variable
    QMap<QString, QStringList> __ArrayNameMap;    //map for names to variables
    vtkPoints *buildGrid();                     //builds the grid regardless of cache
    vtkPoints *getGrid();                       //returns either the cached grid or calls the build grid
    bool __gridClean;                           //provides status of the grid
    vtkPoints *__grid;                            //grid cache
    int* __getCurrentExtents();                   //current data extents that are set
    vtkFloatArray* getDataFromFile(QString arrayName, int block=0);             //retrieves the data from file and places in vtkFloat

    //STATE
    double current_MJD;                         //current timestep

    //3D Data Files
    QMap<double, enlil3DFile*> _3Dfiles;        //3D Grid/data files

    //EVO files
    QMap<QString, enlilEvoFile*> evoFiles;      //environment files
    void addEvoFile(const char* FileName, const char *refName);
    void locateAndLoadEvoFiles();

    vtkEnlilReader(const vtkEnlilReader&);      // Not implemented.
    void operator=(const vtkEnlilReader&);      // Not implemented.
    int checkStatus(void *Object, char *name);

    QPair<double, double> getTimeStepRange();
    bool __eq(int extent1[], int extent2[]);

    int __currentExtents[6];                      // use ONLY with __getCurrentExtents, or it may be inconsistant.
};



#endif // vtkEnlilReader_H
