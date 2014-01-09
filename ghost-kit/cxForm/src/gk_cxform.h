#ifndef GK_CXFORM_H
#define GK_CXFORM_H

#include "vtkPointSetAlgorithm.h"
#include "vtkFiltersGeneralModule.h"
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



class VTKFILTERSGENERAL_EXPORT gk_cxform : public vtkPointSetAlgorithm
{
public:

        static gk_cxform *New();
        vtkTypeMacro(gk_cxform, vtkPointSetAlgorithm)
        void PrintSelf(ostream& os, vtkIndent indent);

        void SetSourceSystem(int value);
        void SetDestSystem(int value);

        unsigned long GetMTime();

protected:
        gk_cxform();

        int RequestDataObject(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
        int RequestData(vtkInformation *request, vtkInformationVector ** inputVector, vtkInformationVector * outputVector);

        int sourceSystem;
        int destSystem;

private:
        gk_cxform(const gk_cxform&);  // Not implemented.
        void operator=(const gk_cxform&);  // Not implemented.
        std::vector<std::string> systemLookupTable;

};

#endif // GK_CXFORM_H
