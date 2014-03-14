#ifndef GK_CXFORMFIELD_H
#define GK_CXFORMFIELD_H

#include "vtkPVClientServerCoreCoreModule.h" //needed for exports
#include "vtkPointSetAlgorithm.h"
#include "vtkFiltersGeneralModule.h"
#include "vtkTableAlgorithm.h"
#include "vtkDataSetAlgorithm.h"
#include "vtkDataArraySelection.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkAlgorithm.h"
#include "vtkStringArray.h"
#include "vtkStringList.h"
#include "vtkCallbackCommand.h"
#include <vector>
#include <string>
#include <QMap>
#include <QString>
#include <QStringList>


#define GK_POINTDATASTR "Point Data"
#define GK_CELLDATASTR  "Cell Data"
#define GK_FIELDDATASTR "Field Data"


class  gk_cxFormField : public vtkTableAlgorithm
{
public:
    static gk_cxFormField *New();
    vtkTypeMacro(gk_cxFormField, vtkTableAlgorithm)

    void PrintSelf(ostream &os, vtkIndent indent);

    //xform setup
    void SetSourceSystem(const char *value);
    vtkStringArray *GetSourceInfo();

    void SetDestSystem(const char *value);
    vtkStringArray *GetDestinationInfo();

    void SetDataSource(const char *value);
    vtkStringArray *GetDataSourceInfo();

    //field selections
    int GetNumberOfTableArrays();
    const char* GetTableArrayName(int index);
    int GetTableArrayStatus(const char* name);
    void SetTableArrayStatus(const char* name, int status);
    void DisableAllTableArrays();
    void EnableAllTableArrays();
    vtkStringArray* GetScalarFieldList();
    vtkStringArray* GetVectorFieldList();
    vtkStringArray* GetTableArrayInfo();

    //manual xform
    void SetManualFromSystem(const char *system);
    void SetManualToSystem(const char *system);
    void setUseManual(int status);
    void SetManualName(char* name);
    void SetManualInput(double x, double y, double z);
    void SetManualOutput(double x, double y, double z);

    //split fields xform
    void SetSplitFromSystem(const char *system);
    void SetSplitToSystem(const char *system);
    void setUseSplit(int status);
    void SetSplitFieldName(char* name);
    void SetSplitX(const char*  x);
    void SetSplitY(const char*  y);
    void SetSplitZ(const char*  z);


protected:
    gk_cxFormField();
    ~gk_cxFormField();

    virtual int FillInputPortInformation(int port, vtkInformation* info);

    virtual int RequestDataObject(vtkInformation* request,
                                  vtkInformationVector** inputVector,
                                  vtkInformationVector* outputVector);

    virtual int RequestData(vtkInformation *request,
                            vtkInformationVector **inputVector,
                            vtkInformationVector *outputVector);

    virtual int RequestInformation(vtkInformation *request,
                                   vtkInformationVector **inputVector,
                                   vtkInformationVector *outputVector);


    void PopulateArrays();



    //track wich systems we are using
    vtkStdString sourceSystem;
    vtkStdString destSystem;


    //field selection data structures
    vtkDataArraySelection* vectorFields;
    vtkDataArraySelection* dataSources;
    vtkDataArraySelection* scalarFields;
    vtkStdString           DataSource;

    //current lists for GUI
    vtkStringArray* currentDataSourceList;
    vtkStringArray* currentScalarFieldsList;
    vtkStringArray* currentVectorFieldsList;

    //tracking vars
    //Split XForm
    vtkStdString    splitFieldName;
    vtkStdString    splitXfield;
    vtkStdString    splitYfield;
    vtkStdString    splitZfield;
    vtkStdString    splitFrom;
    vtkStdString    splitTo;
    int             useSplit;


    //Manual XForm
    vtkStdString    manualFieldName;
    vtkStdString    manualFrom;
    vtkStdString    manualTo;
    int             useManual;
    double          manualX;
    double          manualY;
    double          manualZ;

    //callbacks
    vtkCallbackCommand* VectorObserver;
    vtkCallbackCommand* ScalarObserver;
    vtkCallbackCommand* SourceObserver;
    vtkCallbackCommand* AvailableSystemObserver;

    static void VectorCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
    static void ScalarCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
    static void SourceCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
    static void AvailableSystemCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);


private:
    gk_cxFormField(const gk_cxFormField&);   //Not Implemented
    void operator =(const gk_cxFormField&);  //Not Implemented

    vtkStringArray* availableSystemList;

    QMap<QString, QStringList> AvailableVectorFields;
    QMap<QString, QStringList> AvailableScalarFields;

    void generateVectorStringArray();


};

#endif // GK_CXFORMFIELD_H
