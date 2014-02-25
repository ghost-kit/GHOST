#ifndef GK_CXFORMFIELD_H
#define GK_CXFORMFIELD_H

#include "vtkPVClientServerCoreCoreModule.h" //needed for exports
#include "vtkPointSetAlgorithm.h"
#include "vtkFiltersGeneralModule.h"
#include "vtkTableAlgorithm.h"
#include "vtkDataSetAlgorithm.h"
#include "vtkAlgorithm.h"
#include <vector>

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

    void SetSourceSystem(int value);
    void SetDestSystem(int value);

    void SetInputVariable(char* name);

    void SetOutputValue(double x, double y, double z);
    void SetManualSource(double x, double y, double z);

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

private:
    gk_cxFormField(const gk_cxFormField&);   //Not Implemented
    void operator =(const gk_cxFormField&);  //Not Implemented
    std::vector<std::string> systemLookupTable;


};

#endif // GK_CXFORMFIELD_H
