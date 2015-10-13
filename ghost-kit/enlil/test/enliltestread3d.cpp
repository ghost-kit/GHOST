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






}
