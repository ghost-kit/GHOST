#ifndef GKPLOTSOURCE_H
#define GKPLOTSOURCE_H

#include "vtkStructuredGridAlgorithm.h"
#include "vtkDoubleArray.h"
#include "QMap"

class gkPlotSource : public vtkStructuredGridAlgorithm
{
public:
    static gkPlotSource *New();
    vtkTypeMacro(gkPlotSource, vtkStructuredGridAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    void setGridExtents(const char *extents);
    void setGridPoint(const char *value);
    void setGridRange(const char *range);

protected:
    gkPlotSource();
    ~gkPlotSource();

    vtkDoubleArray* PlotData;
    vtkPoints* PlotGrid;

    int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    int RequestData(vtkInformation *request, vtkInformationVector ** inputVector, vtkInformationVector * outputVector);
    int FillInputPortInformation(int port, vtkInformation *info);
    int FillOutputPortInformation(int port, vtkInformation *info);

    int extents[6];
    int dims[3];
    double range[2];

    void updateData();

private:

    QMap<int, QMap<int, QMap<int, double> > > data;

    gkPlotSource(const gkPlotSource&);
    void operator =(const gkPlotSource&);
};

#endif // GKPLOTSOURCE_H
