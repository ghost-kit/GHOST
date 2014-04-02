#include "gkPlotSource.h"


#include "vtkCommand.h"
#include "vtkAbstractArray.h"
#include "vtkCallbackCommand.h"
#include "vtkCompositeDataIterator.h"
#include "vtkDataSetAttributes.h"
#include "vtkDataArraySelection.h"
#include "vtkCompositeDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkDataArray.h"
#include "vtkDataObject.h"
#include "vtkFieldData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStructuredGridAlgorithm.h"
#include "vtkStructuredGrid.h"
#include "vtkStructuredData.h"
#include "vtkTable.h"
#include "vtksys/SystemTools.hxx"
#include "vtkPointData.h"

#include "qstring.h"
#include "QStringList"

//===============================================//
vtkStandardNewMacro(gkPlotSource)

void gkPlotSource::PrintSelf(ostream &os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

}

void gkPlotSource::setGridExtents(const char* extents)
{
    QString Extents(extents);
    QStringList extentList = Extents.split(",");

    if(extentList.size() == 6)
    {
        std::cout << "Extents: " << extents << std::endl;
        //set the extents
        this->extents[0] = extentList[0].toInt();
        this->extents[1] = extentList[1].toInt();
        this->extents[2] = extentList[2].toInt();
        this->extents[3] = extentList[3].toInt();
        this->extents[4] = extentList[4].toInt();
        this->extents[5] = extentList[5].toInt();
    }

    //set dimensions
    this->dims[0] = this->extents[1]-this->extents[0]+1;
    this->dims[1] = this->extents[3]-this->extents[2]+1;
    this->dims[2] = this->extents[5]-this->extents[4]+1;

    this->data.clear();
    this->PlotGrid->Reset();
    for(int z = this->extents[4]; z <= this->extents[5]; z++)
    {
        for(int y = this->extents[2]; y <= this->extents[3]; y++)
        {
            for(int x = this->extents[0]; x <= this->extents[1]; x++)
            {

                this->data[x][y][z] = 0.0;
                this->PlotGrid->InsertNextPoint(double(x), double(y), double(z));
            }
        }
    }

    this->updateData();
    this->Modified();
}

void gkPlotSource::setGridPoint(const char* value)
{
    QString gridValue = value;
    QStringList valueList = gridValue.split(",");

    if(valueList.size() == 4)
    {
        std::cout << "x: " << valueList[0].toInt() << " y: " << valueList[1].toInt() << " z: " <<valueList[2].toInt() <<  ":: " << valueList[3].toDouble() << std::endl;
        this->data[valueList[0].toInt()][valueList[1].toInt()][valueList[2].toInt()] = valueList[3].toDouble();
        this->updateData();
        this->Modified();

    }
}

void gkPlotSource::setGridRange(const char* range)
{
    QString StringRange(range);
    QStringList parts = StringRange.split(",");
    std::cout << "Range: " << range << std::endl;

    this->range[0] = parts[0].toDouble();
    this->range[1] = parts[1].toDouble();
}

gkPlotSource::gkPlotSource()
    : Superclass()
{
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);

    this->PlotData = vtkDoubleArray::New();
    this->PlotData->SetName("Data");
    this->PlotData->SetNumberOfComponents(1);

    this->PlotGrid = vtkPoints::New();

    this->extents[0] = 0;
    this->extents[1] = 0;
    this->extents[2] = 0;
    this->extents[3] = 0;
    this->extents[4] = 0;
    this->extents[5] = 0;

}

gkPlotSource::~gkPlotSource()
{
    this->PlotData->Delete();
    this->PlotGrid->Delete();

}

int gkPlotSource::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
                 this->extents,
                 6);

    return 1;
}

int gkPlotSource::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

    //get inpput and output inforamation
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkStructuredGrid* output = vtkStructuredGrid::GetData(outputVector);

    output->SetDimensions(this->dims);

    output->SetExtent(this->extents);

    std::cout << "Extents have been set..." << std::endl << std::flush;

    output->SetPoints(this->PlotGrid);
    std::cout << "Grid has been set..." << std::endl << std::flush;

    output->GetPointData()->AddArray(this->PlotData);
    std::cout << "DataTest: " << this->PlotData->GetValue(0) << std::endl;
    std::cout << "Data has been set..." << std::endl << std::flush;


    return 1;
}

int gkPlotSource::FillInputPortInformation(int port, vtkInformation *info)
{

    return Superclass::FillInputPortInformation(port,info);
}

int gkPlotSource::FillOutputPortInformation(int port, vtkInformation *info)
{

    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredGrid");
    return 1;
}

void gkPlotSource::updateData()
{
    int count = 0;
    for(int z = this->extents[4]; z <= this->extents[5]; z++)
    {
        for(int y = this->extents[2]; y <= this->extents[3]; y++)
        {
            for(int x = this->extents[0]; x <= this->extents[1]; x++)
            {
                this->PlotData->InsertValue(count,this->data[x][y][z]);
                count++;
                std::cout << "[" << x << "," << y << "," << z << "]:" << this->PlotData->GetValue(count-1) << std::endl;
            }
        }
    }
    std::cout << "PlotData Size: " << this->PlotData->GetMaxId() << std::endl;

    double range[2];
    this->PlotData->GetValueRange(range);
    std::cout << "PlotData Range: " << range[0] << " - " << range[1] << std::endl;
    this->Modified();
    this->Update(0);

}
