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

//===============================================//
gk_cxform::gk_cxform()
{

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

    vtkDebugMacro(<<"Executing transform filter");

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
        //TODO: Implement the actual transform
//        this->Transform->TransformPointsNormalsVectors(inPts, newPts, inNormals, newNormals, inVectors, newVectors);


        std::cerr << "MJD: " << mjd;
        std::cerr << " : Points, Normals, Vectors to be transformed: " << inVectors->GetName() << std::endl;
    }
    else
    {
        //TODO: Implement the actual transform

        std::cerr << "MJD: " << mjd;
        std::cerr << " : Transforming Grid Only" << std::endl;

        //get date/Time components
        DateTime currentDate(mjd);

        std::vector<double> testVector;
        testVector.push_back(-896921337.28302002);
        testVector.push_back(220296912.43620300);
        testVector.push_back(44419205.01961136);

        //cppForm::cxformpp newXform(currentDate, "GSM", testVector);

        //double *testOut = newXform.cxForm("HEEQ");

        //std::cerr << "Test Output: " << testOut[0] << "," << testOut[1] << "," << testOut[2] << std::endl;


    }

    this->UpdateProgress (.6);


//TODO: Transform Cells

    this->UpdateProgress (.8);

// Update ourselves and release memory
//    output->SetPoints(newPts);
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
