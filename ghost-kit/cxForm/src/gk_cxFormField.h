#ifndef GK_CXFORMFIELD_H
#define GK_CXFORMFIELD_H

#include "vtkPVClientServerCoreCoreModule.h" //needed for exports
#include "vtkPointSetAlgorithm.h"
#include "vtkFiltersGeneralModule.h"
#include "vtkTableAlgorithm.h"
#include "vtkDataSetAlgorithm.h"
#include "vtkDataArraySelection.h"
#include "vtkAlgorithm.h"
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

    //field selections
    int GetNumberOfFieldArrays();
    const char* GetFieldArrayName(int index);
    int GetFieldArrayStatus(const char* name);
    void SetFieldArrayStatus(const char* name, int status);
    void DisableAllFieldArrays();
    void EnableAllFieldArrays();

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


    virtual int FillInputPortInformation(int port, vtkInformation* info);

    virtual int RequestDataObject(vtkInformation* request,
                                  vtkInformationVector** inputVector,
                                  vtkInformationVector* outputVector);

    virtual int RequestData(vtkInformation *request,
                            vtkInformationVector **inputVector,
                            vtkInformationVector *outputVector);

    int sourceSystem;
    int destSystem;



    //field selection data structures
    vtkDataArraySelection* fields;

    int     useSplit;
    int     splitFrom;
    int     splitTo;
    std::string   splitFieldName;
    std::string   splitXfield;
    std::string   splitYfield;
    std::string   splitZfield;

    int     useManual;
    int     manualFrom;
    int     manualTo;
    std::string   manualFieldName;
    double  manualX;
    double  manualY;
    double  manualZ;

private:
    gk_cxFormField(const gk_cxFormField&);   //Not Implemented
    void operator =(const gk_cxFormField&);  //Not Implemented
    std::vector<std::string> systemLookupTable;


};

#endif // GK_CXFORMFIELD_H
