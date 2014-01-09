#include "gk_cxform.h"

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
#include "vtkImageData.h"
#include "vtkNew.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmartPointer.h"
#include "vtkImageDataToPointSet.h"
#include "vtkRectilinearGridToPointSet.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"


#include "ltrDateTime.h"
#include "cppxform.h"
#include <math.h>
#include <vector>

//===============================================//
gk_cxform::gk_cxform()
{
    this->systemLookupTable.push_back(std::string("UNKNOWN"));
    this->systemLookupTable.push_back(std::string("J2000"));
    this->systemLookupTable.push_back(std::string("GEI"));
    this->systemLookupTable.push_back(std::string("GEO"));
    this->systemLookupTable.push_back(std::string("MAG"));
    this->systemLookupTable.push_back(std::string("GSE"));
    this->systemLookupTable.push_back(std::string("GSM"));
    this->systemLookupTable.push_back(std::string("SM"));
    this->systemLookupTable.push_back(std::string("RTN"));
    this->systemLookupTable.push_back(std::string("GSEQ"));
    this->systemLookupTable.push_back(std::string("HEE"));
    this->systemLookupTable.push_back(std::string("HAE"));
    this->systemLookupTable.push_back(std::string("HEEQ"));
    this->systemLookupTable.push_back(std::string("HEEQ-180"));



}

//===============================================//
vtkStandardNewMacro(gk_cxform)

//===============================================//
int gk_cxform::RequestDataObject(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
    vtkImageData *inImage = vtkImageData::GetData(inputVector[0]);
    vtkRectilinearGrid *inRect = vtkRectilinearGrid::GetData(inputVector[0]);

    if (inImage || inRect)
    {
        vtkStructuredGrid *output = vtkStructuredGrid::GetData(outputVector);
        if (!output)
        {
            vtkNew<vtkStructuredGrid> newOutput;
            outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), newOutput.GetPointer());
        }
        return 1;
    }
    else
    {
        return this->Superclass::RequestDataObject(request, inputVector, outputVector);
    }
}



//===============================================//
void gk_cxform::PrintSelf(ostream &os, vtkIndent indent)
{

}

//===============================================//
void gk_cxform::SetSourceSystem(int value)
{
    this->sourceSystem = value;
    this->Modified();

    std::cerr << "Modified Source System to " << value << std::endl;

}

//===============================================//
void gk_cxform::SetDestSystem(int value)
{

    this->destSystem = value;
    this->Modified();

    std::cerr << "Modified Destination System to " << value << std::endl;
}

//===============================================//
int gk_cxform::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{

    vtkInformation* info = outputVector->GetInformationObject(0);

    //get MJD
    double mjd = 0.0;

    //get time request data
    if(info->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()))
    {
        mjd = info->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
    }

    vtkSmartPointer<vtkPointSet> input = vtkPointSet::GetData(inputVector[0]);
    vtkPointSet *output = vtkPointSet::GetData(outputVector);

    if (!input)
    {
        // Try converting image data.
        vtkImageData *inImage = vtkImageData::GetData(inputVector[0]);
        if (inImage)
        {
            vtkNew<vtkImageDataToPointSet> image2points;
            image2points->SetInputData(inImage);
            image2points->Update();
            input = image2points->GetOutput();
        }
    }

    if (!input)
    {
        // Try converting rectilinear grid.
        vtkRectilinearGrid *inRect = vtkRectilinearGrid::GetData(inputVector[0]);
        if (inRect)
        {
            vtkNew<vtkRectilinearGridToPointSet> rect2points;
            rect2points->SetInputData(inRect);
            rect2points->Update();
            input = rect2points->GetOutput();
        }
    }

    if (!input)
    {
        vtkErrorMacro(<< "Invalid or missing input");
        return 0;
    }

    vtkPoints *inPts;
    vtkPoints *newPts;
    vtkDataArray *inVectors, *inCellVectors;;
    vtkFloatArray *newVectors=NULL, *newCellVectors=NULL;
    vtkDataArray *inNormals, *inCellNormals;
    vtkFloatArray *newNormals=NULL, *newCellNormals=NULL;
    vtkIdType numPts, numCells;
    vtkPointData *pd=input->GetPointData(), *outPD=output->GetPointData();
    vtkCellData *cd=input->GetCellData(), *outCD=output->GetCellData();

    vtkDebugMacro(<<"Executing cxform filter");

    // First, copy the input to the output as a starting point
    output->CopyStructure( input );

    // Check input
    inPts = input->GetPoints();
    inVectors = pd->GetVectors();
    inNormals = pd->GetNormals();
    inCellVectors = cd->GetVectors();
    inCellNormals = cd->GetNormals();

    if ( !inPts )
    {
        vtkErrorMacro(<<"No input data");
        return 1;
    }

    numPts = inPts->GetNumberOfPoints();
    numCells = input->GetNumberOfCells();

    newPts = vtkPoints::New();
    newPts->Allocate(numPts);
    if ( inVectors )
    {
        newVectors = vtkFloatArray::New();
        newVectors->SetNumberOfComponents(3);
        newVectors->Allocate(3*numPts);
        newVectors->SetName(inVectors->GetName());
    }
    if ( inNormals )
    {
        newNormals = vtkFloatArray::New();
        newNormals->SetNumberOfComponents(3);
        newNormals->Allocate(3*numPts);
        newNormals->SetName(inNormals->GetName());
    }

    this->UpdateProgress (.2);

    // Loop over all points, updating position
    //

    if ( inVectors || inNormals )
    {
        //TODO: Implement the actual non-structured transformation
        vtkErrorMacro(<< "Non-Structured input not yet supported.");

    }
    else
    {
        //TODO: Implement the actual transform

        std::cerr << "MJD: " << mjd;
        std::cerr << " : Transforming Structured Grid" << std::endl;

        std::cerr << "InSystem:  " << this->systemLookupTable[this->sourceSystem] << std::endl;
        std::cerr << "OutSystem: " << this->systemLookupTable[this->destSystem] << std::endl;

        //transform points
        double xyz[3];
        DateTime xformDate(mjd);
        double *xyzxform;
        std::vector<double> point;

        for(int x = 0; x < numPts; x ++)
        {
            //get points
            inPts->GetPoint(x, xyz);
            point.clear();

            //transform point
            cppForm::cxformpp xform(xformDate, this->systemLookupTable[this->sourceSystem].c_str(), xyz);
            xyzxform = xform.cxForm(this->systemLookupTable[this->destSystem].c_str());


            newPts->InsertPoint(x,xyzxform);
        }

        this->Modified();
    }

    this->UpdateProgress (.6);


//TODO: Transform Cells

    this->UpdateProgress (.8);

// Update ourselves and release memory
    output->SetPoints(newPts);
    newPts->Delete();

    if (newNormals)
      {
      outPD->SetNormals(newNormals);
      newNormals->Delete();
      outPD->CopyNormalsOff();
      }

    if (newVectors)
      {
      outPD->SetVectors(newVectors);
      newVectors->Delete();
      outPD->CopyVectorsOff();
      }

    if (newCellNormals)
      {
      outCD->SetNormals(newCellNormals);
      newCellNormals->Delete();
      outCD->CopyNormalsOff();
      }

    if (newCellVectors)
      {
      outCD->SetVectors(newCellVectors);
      newCellVectors->Delete();
      outCD->CopyVectorsOff();
      }

    outPD->PassData(pd);
    outCD->PassData(cd);

    vtkFieldData* inFD = input->GetFieldData();
    if (inFD)
      {
      vtkFieldData* outFD = output->GetFieldData();
      if (!outFD)
        {
        outFD = vtkFieldData::New();
        output->SetFieldData(outFD);
        // We can still use outFD since it's registered
        // by the output
        outFD->Delete();
        }
      outFD->PassData(inFD);
      }

    this->UpdateProgress (1.0);

    this->Modified();
    return 1;
}

unsigned long gk_cxform::GetMTime()
{
    unsigned long mTime=this->MTime.GetMTime();
    unsigned long transMTime;

    //  if ( this->Transform )
    //    {
    //    transMTime = this->Transform->GetMTime();
    //    mTime = ( transMTime > mTime ? transMTime : mTime );
    //    }

    return mTime;
}
