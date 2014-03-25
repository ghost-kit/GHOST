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
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDataObjectTypes.h"

#include "ltrDateTime.h"

#include <vector>
#include <string>
#include <QMap>
#include <QString>
#include <QStringList>


#define GK_POINTDATASTR "Point Data"
#define GK_CELLDATASTR  "Cell Data"
#define GK_FIELDDATASTR "Field Data"
#define GK_MODELTIMESTR "Model Time Data"


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

    //Time Selection Array
    //  Mode 0) only Model Time
    //  Mode 1) only Scalar Arrays
    //  Mode 2) Scalar Arrays and Model time
    void setTimeArray(int mode);
    void setUseModelTime(int status);
    void setManualDate(int MM, int DD, int YYYY);
    void setManualTime(int hour, int min, int sec);
    vtkStringArray* GetTimeFields();

    //field selections
    int GetNumberOfTableArrays();
    const char* GetTableArrayName(int index);
    int GetTableArrayStatus(const char* name);
    void SetArraySelectionStatus(const char* name, int status);
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
    void SetXformTime(const char *timeArray);
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
    int                    useModelTime;

    //current lists for GUI
    vtkStringArray* currentDataSourceList;
    vtkStringArray* currentScalarFieldsList;
    vtkStringArray* currentVectorFieldsList;

    //tracking vars
    //Split XForm
    vtkStdString    xformTimeField;
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
    DateTime        manualDate;
    int             useManual;
    double          manualX;
    double          manualY;
    double          manualZ;

    //callbacks
    vtkCallbackCommand* VectorObserver;
    vtkCallbackCommand* ScalarObserver;
    vtkCallbackCommand* SourceObserver;
    vtkCallbackCommand* AvailableSystemObserver;
    vtkCallbackCommand* AvailableTimeFieldsObserver;

    static void VectorCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
    static void ScalarCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
    static void SourceCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
    static void AvailableSystemCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);
    static void AvailableTimeFieldsCallback(vtkObject *caller, unsigned long eid, void *clientdata, void *calldata);


    //conversion routines
    void convertVectorData(DateTime inputDate, QString name, vtkTable* outputTable, vtkFieldData *currentData);
    void convertVectorData(DateTime inputDate, QString name, vtkTable* outputTable, vtkTable *currentData);

    void convertSplitData(QString time, QString nameX, QString nameY, QString nameZ, vtkTable* outputTable, vtkFieldData *currentData);
    void convertSplitData(QString time, QString nameX, QString nameY, QString nameZ, vtkTable* outputTable, vtkTable *currentData);

    vtkDoubleArray* convertDoubleData(QString name, vtkDoubleArray* oldData, DateTime inputDate);
    vtkDoubleArray* convertDoubleData(QString name, vtkDoubleArray* oldData, vtkDoubleArray* inputDate);

    vtkFloatArray*  convertFloatData( QString name, vtkFloatArray* oldData, DateTime inputDate);
    vtkFloatArray*  convertFloatData( QString name, vtkFloatArray* oldData, vtkDoubleArray* inputDate);

    vtkDoubleArray* getAsDouble(QString ArrayName, vtkFieldData* dataSource);
    vtkDoubleArray* getAsDouble(QString ArrayName, vtkTable *dataSource);

private:
    gk_cxFormField(const gk_cxFormField&);   //Not Implemented
    void operator =(const gk_cxFormField&);  //Not Implemented

    vtkStringArray* availableSystemList;
    vtkStringArray* availableScalarsForTimeSelection;

    QMap<QString, QStringList> AvailableVectorFields;
    QMap<QString, QStringList> AvailableScalarFields;

    void generateVectorStringArray();

    //data pointers
    QMap<QString, vtkPointData*> pdVector;
    QMap<QString, vtkCellData*>  cdVector;
    QMap<QString, vtkFieldData*> fdVector;
    QMap<QString, vtkTable*>     tdVector;

    double currentMJD;


};

#endif // GK_CXFORMFIELD_H
