#ifndef GKPLOTSOURCE_H
#define GKPLOTSOURCE_H

#include "vtkStructuredGridAlgorithm.h"
#include "vtkDoubleArray.h"

class gkPlotSource : public vtkStructuredGridAlgorithm
{
public:
    static gkPlotSource *New();
    vtkTypeMacro(gkPlotSource, vtkStructuredGridAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkDoubleArray* getPlotData();
    vtkDoubleArray* getPlotGrid();

protected:
    gkPlotSource();
    ~gkPlotSource();

    vtkDoubleArray* PlotData;
    vtkDoubleArray* PlotGrid;

    int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    int RequestData(vtkInformation *request, vtkInformationVector ** inputVector, vtkInformationVector * outputVector);
    int FillInputPortInformation(int port, vtkInformation *info);
    int FillOutputPortInformation(int port, vtkInformation *info);


private:
    gkPlotSource(const gkPlotSource&);
    void operator =(const gkPlotSource&);
};

#endif // GKPLOTSOURCE_H
