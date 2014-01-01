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


protected:
        gk_cxform();

};

#endif // GK_CXFORM_H
