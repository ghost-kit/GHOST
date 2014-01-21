#include "cppxform.h"
#include "iostream"
#include "ltrDateTime.h"
#include "vector"
#include "string"


int main()
{
    DateTime newDate(56002.021);
    double inVector[3];


    inVector[0] = 81200.0;
    inVector[1] = 2120.0;
    inVector[2] = 12120.0;

    cppForm::cppxform testUnit(newDate, "HEEQ180", inVector);
    double* outVector = testUnit.cxForm("HEEQ");

    std::cout << "Date: " << newDate.getDateTimeString() << std::endl;

    std::cout << "ES cxForm Style: " << std::endl;
    std::cout << "Input Vector:  " << inVector[0] << "," << inVector[1] << "," << inVector[2] << " (HEEQ180)" << std::endl;
    std::cout << "Output Vector: " << outVector[0] << "," << outVector[1] << "," << outVector[2] << " (HEEQ)" << std::endl << std::endl;

    cppForm::cppxform testUnit2(newDate, "HEEQ", outVector);
    outVector = testUnit2.cxForm("HEEQ180");

     std::cout << "Reversal: " << std::endl;
    std::cout << "Output Vector: " << outVector[0] << "," << outVector[1] << "," << outVector[2] << " (HEEQ180)" << std::endl << std::endl;



}
