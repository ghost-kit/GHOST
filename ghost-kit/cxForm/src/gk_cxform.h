#ifndef GK_CXFORM_H
#define GK_CXFORM_H

#include "vtkPointSetAlgorithm.h"
#include "vtkFiltersGeneralModule.h"

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


private:
        gk_cxform(const gk_cxform&);  // Not implemented.
        void operator=(const gk_cxform&);  // Not implemented.

};

#endif // GK_CXFORM_H
