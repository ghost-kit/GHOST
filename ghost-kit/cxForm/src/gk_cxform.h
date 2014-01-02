#ifndef GK_CXFORM_H
#define GK_CXFORM_H

#include "vtkDataSetAlgorithm.h"
#include "vtkFiltersExtractionModule.h"

class VTKFILTERSEXTRACTION_EXPORT gk_cxform : public vtkDataSetAlgorithm
{
public:

        static gk_cxform *New();
        vtkTypeMacro(gk_cxform, vtkDataSetAlgorithm)
        void PrintSelf(ostream& os, vtkIndent indent);

        void SetSourceSystem(int value);
        void SetDestSystem(int value);

protected:
        gk_cxform();

        int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
        int RequestData(vtkInformation *request, vtkInformationVector ** inputVector, vtkInformationVector * outputVector);
        int FillInputPortInformation(int port, vtkInformation *info);
        int FillOutputPortInformation(int port, vtkInformation *info);

};

#endif // GK_CXFORM_H
