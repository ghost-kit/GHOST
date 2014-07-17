#ifndef LFM_HDF5_READER_H
#define LFM_HDF5_READER_H

#include "vtkStructuredGridAlgorithm.h"
#include "vtkIOExportModule.h"
#include "vtkNew.h"
#include "vtkFloatArray.h"
#include "vtkDataArraySelection.h"
#include "vtkSMSourceProxy.h"
#include "vtkSetGet.h"
#include "vtk_hdf5.h"
#include "vtkExporter.h"

#include "QString"
#include "QStringList"
#include "QVector"

#include "ltrDateTime.h"

class VTKIOEXPORT_EXPORT_H lfm_hdf5_reader : public vtkStructuredGridAlgorithm
{
public:
    static lfm_hdf5_reader* New();

    vtkTypeMacro(lfm_hdf5_reader, vtkStructuredGridAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    virtual void AddFileName(const char *fileName);
    virtual void RemoveAllFileNames();
    virtual char* GetActiveFileName();


protected:
    lfm_hdf5_reader();
    ~lfm_hdf5_reader();


    //required implementations
    virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

    //additional public helpers
    int getNumberOfTimeSteps();
    double getTimeStep(int index);

private:

    //File Name tracking
    QString activeFileName;
    QStringList allFileNames;

    //Time Steps
    QVector<double> TimeStepValues;

    //Reset all file data
    void resetAllData();

    //Variable Selections
    vtkDataArraySelection* PointDataArraySelection;
    vtkDataArraySelection* CellDataArraySelection;

    //True if something has changed
    bool dirty;



};

#endif // LFM_HDF5_READER_H
