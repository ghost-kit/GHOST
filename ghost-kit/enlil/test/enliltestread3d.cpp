/**
  *This test program tests the operation of the enlil3dFile Reader class.  It requires NetCDF to operate.
  *
  **/

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "../src/enlil3dfile.h"
#include "../src/enlilEvoFile.h"

#include <QString>
#include <cmath>
#include <limits>
#include "testhelpers.h"

#define _EVOTESTFILE "evo.earth.nc"
#define _3DTESTFILE "tim.0038.nc"


int main(int argc, char* argv[])
{
    int loop = 0;
    std::cout << "This is a test of the Enlil 3D File Reader C++ class for Paraview." << std::endl;

    /**
     * Strat Testing File
     */
    QString testFile = QString(_3DTESTFILE);

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
    enlilAtt* refrenceDate = file.getFileAttribute("refdate_mjd");
    if(!refrenceDate) refrenceDate = file.getFileAttribute("rundate_mjd");
    if(!refrenceDate)
    {
        std::cerr << "ERROR: Reference Date Not Found" << std::endl;
        exit(EXIT_FAILURE);
    }

    double refdate = refrenceDate->getValue().toDouble();

    std::cout << "MJD: " << MJD << std::endl;
    std::cout << "refDate: " << refdate << std::endl;

    std::cout << "File is " << MJD - refdate << " Days from the Reference date." << std::endl;


    /**
      *Test GetVar and Dims
      */
    int xyz[3];
    xyz[0] = file.getDims("n1");
    xyz[1] = file.getDims("n2");
    xyz[2] = file.getDims("n3");

    QVector<double> data1 = file.asDouble("X1");
    QVector<double> data2 = file.asDouble("X2");
    QVector<double> data3 = file.asDouble("X3");

    std::cout << "Size of X1: " << data1.size() << std::endl;
    std::cout << "Size of X2: " << data2.size() << std::endl;
    std::cout << "Size of X3: " << data3.size() << std::endl;

    if(xyz[0] != data1.size())
    {
        std::cerr << "Problem with Dimension X" << std::endl;
        std::cerr << "Expecting " << xyz[0] << std::endl;
        std::cerr << "Received " << data1.size() << std::endl;
        //exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Dimension X Passed" << std::endl;
    }

    if(xyz[1] != data2.size())
    {
        std::cerr << "Problem with Dimension Y" << std::endl;
        std::cerr << "Expecting " << xyz[1] << std::endl;
        std::cerr << "Received " << data2.size() << std::endl;
        //exit(EXIT_FAILURE);

    }
    else
    {
        std::cout << "Dimension Y Passed" << std::endl;
    }

    if(xyz[2] != data3.size())
    {
        std::cerr << "Problem with Dimension Z" << std::endl;
        std::cerr << "Expecting " << xyz[2] << std::endl;
        std::cerr << "Received " << data3.size() << std::endl;
        exit(EXIT_FAILURE);

    }
    else
    {
        std::cout << "Dimension Z Passed" << std::endl;
    }

    /**
      *Test getting 3D data
      */

    if(xyz[0]*xyz[1]*xyz[2] != file.get3Dcount())
    {
        std::cerr << "Dimensional Miss-match with get3Dcount()" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "get3Dcount() appears to be correct." << std::endl;
    }

    QVector<double> densityData = file.asDouble("D");
    int densityCount = densityData.count();

    if(densityCount != file.get3Dcount())
    {
        std::cerr << "Error in Reading 3D data... wrong number of entries read: Read "
                    << densityCount << "Should have Been: "<< xyz[0] * xyz[1] * xyz[2] << std::endl;

        exit(EXIT_FAILURE);

    }
    else
    {
        std::cout << "Succesfully Read " << densityCount << " out of " << xyz[0] * xyz[1] * xyz[2]
                     << " Data points on the 3D Density Scalar Array." << std::endl;
    }


    /**
      * Test grid positions
      **/
    QVector<QVector< double > > gridSpace = file.getGridSpacing();

    if(gridSpace.count() != file.get3Dcount())
    {
        std::cerr << "ERROR... Full grid space does not equal the size of data sets." << std::endl;

        exit(EXIT_FAILURE);

    }
    else
    {
        std::cout << "Grid Space is of the Correct Size." << std::endl;
    }

    int loopx=0;
    int loopy=0;
    int loopz=0;
    int sizex = xyz[0];
    int sizey = xyz[1];
    int sizez = xyz[2];
    loop = 0;

    file.setGridSpacingType(ENLIL_GRIDSPACING_SP);
    gridSpace = file.getGridSpacing();

    for(loopz = 0; loopz < sizez; loopz++ )
    {
        for(loopy = 0; loopy < sizey; loopy++)
        {
            for(loopx = 0; loopx < sizex; loopx++)
            {
                double grid1[3];
                grid1[0] = gridSpace[loop][0];
                grid1[1] = gridSpace[loop][1];
                grid1[2] = gridSpace[loop][2];

                double grid2[3];
                grid2[0] = data1[loopx];
                grid2[1] = data2[loopy];
                grid2[2] = data3[loopz];

                if(grid1[0] != grid2[0] || grid1[1] != grid2[1] || grid1[2] != grid2[2])
                {
                    std::cerr << "Failure on Grid Data [" << loopx << "," << loopy << "," << loopz << "]" << std::endl;
                    std::cerr << "Expected: " << grid2[0]<< "," << grid2[1] << "," << grid2[2] << std::endl;
                    std::cerr << "Received: " << grid1[0] << "," << grid1[1] << "," << grid1[2] << std::endl;


                    exit(EXIT_FAILURE);
                }


                loop++;
            }
        }
    }

    file.setGridSpacingType(ENLIL_GRIDSPACING_CT);

    gridSpace = file.getGridSpacing();

    loop=0;
    double epsilon = 2*std::numeric_limits<double>::epsilon();

    for(loopz = 0; loopz < sizez; loopz++ )
    {
        for(loopy = 0; loopy < sizey; loopy++)
        {
            for(loopx = 0; loopx < sizex; loopx++)
            {
                double grid1[3];
                grid1[0] = gridSpace[loop][0];
                grid1[1] = gridSpace[loop][1];
                grid1[2] = gridSpace[loop][2];

                double calcRadius = sqrt(pow(grid1[0],2) + pow(grid1[1],2) + pow(grid1[2],2));

                double grid2 = data1[loopx];

                if(!doubleCompareEqual(calcRadius, grid2, epsilon ))
                {
                    std::cerr << "Failure on Grid Location [" << loopx << "," << loopy << "," << loopz << "]" << std::endl;
                    std::cerr << "Expected: " << grid2 << std::endl;
                    std::cerr << "Received: " << calcRadius << std::endl;
                    std::cerr << "Tollerance: " << epsilon << std::endl;

                    exit(EXIT_FAILURE);
                }

                loop++;
            }
        }
    }

    std::cout << "Cartesian Grid Values are Consistant to within " << epsilon << " of the Spherical Coordinates for the calculated units" << std::endl;

    /**
      * Test Data Aqcuisition
      *
      **/
    QVector<QVector<double> > VectorData = file.asDouble("V1", "V2", "V3");
    std::cout << "Velocity Vector Size: " << VectorData.count() << std::endl;

    if(VectorData.count() != file.get3Dcount())
    {
        std::cerr << "Velocity Vector Size is Incorrect." << std::endl;
        std::cerr << "Expected: " << file.get3Dcount() << std::endl;
        std::cerr << "Received: " << VectorData.count() << std::endl;
    }
    else
    {
        std::cout << "Velocity Vector Size is Correct." << std::endl;
    }

    QVector<QVector<double> > VectorData2 = file.asDouble("B1", "B2", "B3");
    std::cout << "B Field Vector Size: " << VectorData2.count() << std::endl;
    if(VectorData2.count() != file.get3Dcount())
    {
        std::cerr << "B Field Vector Size is Incorrect." << std::endl;
        std::cerr << "Expected: " << file.get3Dcount() << std::endl;
        std::cerr << "Received: " << VectorData2.count() << std::endl;
    }
    else
    {
        std::cout << "B Field Vector Size is Correct." << std::endl;
    }


    /**
      * Test HyperCube retreval
      **/
//    file.setExtents(4, 8, 1, 1, 0, 0);
    QVector<QVector< double > > gridSpaceExtents = file.getGridSpacing();
    int gridExentsSize = file.get3Dcount();

    if(gridSpaceExtents.count() != gridExentsSize)
    {
        std::cerr << "Error in Array Counts for Extents" << std::endl;
        std::cerr << "Expected: " << gridSpaceExtents.count() << std::endl;
        std::cerr << "Received: " << gridExentsSize << std::endl;
    }
    else
    {
        std::cout << "grid 3D total points: " << file.get3Dcount() << std::endl;
        std::cout << "Dims: [" << file.getDims("n1") << "," << file.getDims("n2") << "," << file.getDims("n3") << "]" << std::endl;

    }

    QVector<QVector<double> > Bextent = file.asDouble("B1", "B2", "B3");

    loop=0;
    for(loop=0; loop < file.get3Dcount(); loop++)
    {
        std::cout << "Value: " << Bextent[loop][0] << "," << Bextent[loop][1] << "," << Bextent[loop][2] << std::endl;
    }



    /**
      *     TEST THE EVO FILES
      */

    enlilEvoFile evoFile(_EVOTESTFILE, 1);


}
