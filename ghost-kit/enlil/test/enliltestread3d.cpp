/**
  *This test program tests the operation of the enlil3dFile Reader class.  It requires NetCDF to operate.
  *
  **/

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "../src/enlil3dfile.h"
#include <QString>

int main(int argc, char* argv[])
{
    int loop = 0;
    std::cout << "This is a test of the Enlil 3D File Reader C++ class for Paraview." << std::endl;

    if(argc < 2)
    {
        std::cerr << "A File Name is Required in order to proceed.  The proper syntax is:" << std::endl
                  << argv[0] << " <enlil 3D file name>" << std::endl;
    }

    /**
     * Strat Testing File
     */
    QString testFile = QString(argv[1]);

    std::cout << "Using File: " << qPrintable(testFile) << " to process tests...." << std::endl;
    std::cout << "I am assuming the file is correct, as this is just a test routine." << std::endl;


    std::cout << std::endl << "Testing File Load..." << std::endl;
    enlil3DFile file(qPrintable(testFile),1);

    /**
     * Test Variable Counts
     */

    qint64 numVars = file.getNumberOfVars();
    QStringList Variables = file.getVarNames();
    if(numVars != qint64(Variables.count()))
    {
        std::cout << "Error in the number of vars reported..." << std::endl;
        std::cout << "getNumberOfVars() reports " << numVars << " variables." << std::endl;
        std::cout << "getVarNames() returns " << Variables.count() << " variable names." << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "SUCCESS.... Both methods returned " << numVars << " Variables." << std::endl;
    }

    /**
      * Test Attribute Counts
      */
    int numAttributes = file.getNumberOfFileAttributes();
    QStringList AttributeNames = file.getFileAttributeNames();

    if(numAttributes != AttributeNames.count())
    {
        std::cerr << "Error in number of File Attributes Reported..." << std::endl;
        std::cerr << "getNumberOfFileAttributes() returned " << numAttributes << " Attributes" << std::endl;
        std::cerr << "getFileAttributeNames() returned " << AttributeNames.count() << " Names" << std::endl;
    }
    else
    {
        std::cout << "SUCCESS.... Both methods returned " << numAttributes << " File Bound Attributes." << std::endl;
    }

    /**
      * Test Variable Attributes
      */
    std::cout << "Visually Confirm the Variable Attributes..." << std::endl;
    for(loop=0; loop < numVars; loop++ )
    {
        std::cout << "Var: [" << qPrintable(Variables[loop])<< "]" << std::endl;
        std::cout << "Long Name: [" << qPrintable(file.getVarLongName(qPrintable(Variables[loop])))
                  << "]  Units: [" << qPrintable(file.getVarUnits(qPrintable(Variables[loop]))) << "]" << std::endl;
    }


    /**
      * Test MJD retreval
      */
    double MJD = file.getMJD();
    double refdate = file.getFileAttribute("refdate_mjd").toDouble(0);

    std::cout << "MJD: " << MJD << std::endl;
    std::cout << "refDate: " << refdate << std::endl;

    std::cout << "File is " << MJD - refdate << " Days from the Reference date." << std::endl;


    /**
      *Test GetVar and Dims
      */
    int xyz[3];
    xyz[0] = file.getDims(0);
    xyz[1] = file.getDims(1);
    xyz[2] = file.getDims(2);

    QVector<double> data1 = file.getVar("X1");
    QVector<double> data2 = file.getVar("X2");
    QVector<double> data3 = file.getVar("X3");

    std::cout << "Size of X1: " << data1.size() << std::endl;
    std::cout << "Size of X2: " << data2.size() << std::endl;
    std::cout << "Size of X3: " << data3.size() << std::endl;

    if(xyz[0] != data1.size())
    {
        std::cerr << "Problem with Dimension X" << std::endl;
    }
    else
    {
        std::cout << "Dimension X Passed" << std::endl;
    }

    if(xyz[1] != data2.size())
    {
        std::cerr << "Problem with Dimension Y" << std::endl;
    }
    else
    {
        std::cout << "Dimension Y Passed" << std::endl;
    }

    if(xyz[2] != data3.size())
    {
        std::cerr << "Problem with Dimension Z" << std::endl;
    }
    else
    {
        std::cout << "Dimension Z Passed" << std::endl;
    }

    /**
      *Test getting 3D data1
      */

    QVector<double> densityData = file.getVar("D");
    int densityCount = densityData.count();

    if(densityCount != xyz[0] * xyz[1] * xyz[2])
    {
        std::cerr << "Error in Reading 3D data... wrong number of entries read: Read "
                    << densityCount << "Should have Been: "<< xyz[0] * xyz[1] * xyz[2] << std::endl;

    }
    else
    {
        std::cout << "Succesfully Read " << densityCount << " out of " << xyz[0] * xyz[1] * xyz[2]
                     << " Data points on the 3D Density Scalar Array." << std::endl;
    }


    /**
      * Test grid positions
      **/
    QVector<double*> *gridSpace = file.getGridSpacing();
    if(!gridSpace)
    {
        std::cerr << "ERROR: Grid Spacing Not properly defined." << std::endl;
    }
    else
    {
        std::cout << "Grid Spacing appears to be defined." << std::endl;
    }

    if(gridSpace->count() != file.get3Dcount())
    {
        std::cerr << "ERROR... Full grid space does not equal the size of data sets." << std::endl;

    }
    else
    {
        std::cout << "Grid Space is of the Correct Size." << std::endl;
    }





}
