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
    this->systemLookupTable.push_back(std::string("HEEQ180"));

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

    std::cerr << "Modified Source System to " << this->systemLookupTable[value] << std::endl;

}

//===============================================//
void gk_cxform::SetDestSystem(int value)
{

    this->destSystem = value;
    this->Modified();

    std::cerr << "Modified Destination System to " << this->systemLookupTable[value] << std::endl;
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
    if ( inVectors || inNormals )
    {
        /*Non-Structured Data*/
        //TODO: Implement the actual non-structured transformation
        vtkErrorMacro(<< "Non-Structured input not yet supported with the cxForm Filter.");

    }
    else
    {
        /*Structured Data*/
        vtkDebugMacro(<< "Converting from " << this->systemLookupTable[this->sourceSystem] << " to " << this->systemLookupTable[this->destSystem] << ".");

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
            cppForm::cppxform xform(xformDate, this->systemLookupTable[this->sourceSystem].c_str(), xyz);
            xyzxform = xform.cxForm(this->systemLookupTable[this->destSystem].c_str());

            newPts->InsertPoint(x,xyzxform);

            xyzxform = NULL;
        }

        // Update points and release temp memory
        output->SetPoints(newPts);
        newPts->Delete();

        vtkDebugMacro( << "Tranforming any present Vector Point Data");

        //Tranform Point Data
        int numArrays = pd->GetNumberOfArrays();

        for(int h = 0; h < numArrays; h++)
        {

            std::cerr << "Inside of array loop" << std::endl << std::flush;
            //configure arrays
            vtkDataArray* InArray = pd->GetArray(h);
            vtkDoubleArray* OutArray = vtkDoubleArray::New();

            std::cerr << "Array's Defined and created" << std::endl;

            int numElements = InArray->GetNumberOfTuples();
            int numComponents = InArray->GetNumberOfComponents();

            std::cerr << "Computed Number of elements" << std::endl;

            OutArray->SetNumberOfComponents(numComponents);
            OutArray->Allocate(numComponents*numElements);
            OutArray->SetName(InArray->GetName());

            std::cerr << "Configured output satistics" << std::endl;

            if(pd->GetArray(h)->GetNumberOfComponents() == 3)
            {
                std::cerr << "Verified dealing with a vector" << std::endl;

                double zero[3] = {0,0,0};
                cppForm::cppxform xform(xformDate, this->systemLookupTable[this->sourceSystem].c_str(), zero);
                for(int a = 0; a < numElements; a++)
                {
                    InArray->GetTuple(a, xyz);
                    xform.setInVector(xyz);
                    xyzxform = xform.cxForm(this->systemLookupTable[this->destSystem].c_str());

                    OutArray->InsertNextTuple(xyzxform);

                    //                    std::cerr << "IN:  " << xyz[0] << "," << xyz[1] << "," << xyz[2] << std::endl;
                    //                    std::cerr << "OUT: " << xyzxform[0] << "," << xyzxform[1] << "," << xyzxform[2] << std::endl;

                    xform.cleanHandler();
                }

            }
            else if(pd->GetArray(h)->GetNumberOfComponents() == 1)
            {
                std::cerr << "Scalar Array - unimplemented as yet" << std::endl;

                //TODO: copy scalar arrays
                double inValue;
                for(int a = 0; a < numElements; a++)
                {
                    OutArray->InsertNextTuple1(InArray->GetTuple1(a));

                }
            }
            else
            {
                std::cerr << "only scalar and 3-vectors are supported at this point" << std::endl;
            }

            //update the array
            outPD->AddArray(OutArray);
            OutArray->Delete();

            std::cerr << "Deleted Tempoary Memory" << std::endl;
        }

    }

    this->UpdateProgress (.6);


    //TODO: Transform Cells
    //vtkWarningMacro(<< "We have not yet implemented Cell Data Transformation");

    this->UpdateProgress (.8);


    std::cerr << "Cleaning up ... " << std::endl;


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


    //TODO: check field data manipulations to set the units/coordinate system properly.
    vtkFieldData* inFD = input->GetFieldData();
    if (inFD)
    {
//        vtkFieldData* outFD = output->GetFieldData();
//        if (!outFD)
//        {
//            outFD = vtkFieldData::New();
//            output->SetFieldData(outFD);
//            // We can still use outFD since it's registered
//            // by the output
//            outFD->Delete();
//        }
//        outFD->PassData(inFD);
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
