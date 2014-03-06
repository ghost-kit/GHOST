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
#include <vector>
#include <string>

#define UNKOWN  0
#define J2000   1
#define GEI     2
#define GEO     3
#define MAG     4
#define GSE     5
#define GSM     6
#define SM      7
#define RTN     8
#define GSEQ    9
#define HEE     10
#define HAE     11
#define HEEQ    12
#define HEEQ180 13

class  gk_cxFormField : public vtkTableAlgorithm
{
public:
    static gk_cxFormField *New();
    vtkTypeMacro(gk_cxFormField, vtkTableAlgorithm)

    void PrintSelf(ostream &os, vtkIndent indent);

    //xform setup
    void SetSourceSystem(int value);
    void SetDestSystem(int value);
    void SetDataSource(const char *value);
    void PopulateVectorArrays();
    void PopulateScalarArrays();

    //setup helper methods (for Properties)
    vtkStringArray *GetDataSourceInfo();

    //field selections
    int GetNumberOfTableArrays();
    const char* GetTableArrayName(int index);
    int GetTableArrayStatus(const char* name);
    void SetTableArrayStatus(const char* name, int status);
    void DisableAllTableArrays();
    void EnableAllTableArrays();

    //manual xform
    void SetManualFromSystem(int system);
    void SetManualToSystem(int system);
    void setUseManual(int status);
    void SetManualName(char* name);
    void SetManualInput(double x, double y, double z);
    void SetManualOutput(double x, double y, double z);

    //split fields xform
    void SetSplitFromSystem(int system);
    void SetSplitToSystem(int system);
    void setUseSplit(int status);
    void SetSplitFieldName(char* name);
    void SetSplitX(int x);
    void SetSplitY(int y);
    void SetSplitZ(int z);


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
    int sourceSystem;
    int destSystem;



    //field selection data structures
    vtkDataArraySelection* vectorFields;
    vtkDataArraySelection* dataSources;
    vtkDataArraySelection* scalarFields;
    int DataSource;

    //temp strings
    vtkStringArray* currentDataSourceList;
    vtkStringArray* currentScalarFieldsList;

    //tracking vars
    //Split XForm
    std::string   splitFieldName;
    std::string   splitXfield;
    std::string   splitYfield;
    std::string   splitZfield;
    int     useSplit;
    int     splitFrom;
    int     splitTo;

    //Manual XForm
    std::string   manualFieldName;
    int     useManual;
    int     manualFrom;
    int     manualTo;
    double  manualX;
    double  manualY;
    double  manualZ;

private:
    gk_cxFormField(const gk_cxFormField&);   //Not Implemented
    void operator =(const gk_cxFormField&);  //Not Implemented

    std::vector<std::string> systemLookupTable;


};

#endif // GK_CXFORMFIELD_H
