#include "cppxform.h"
#include "iostream"
#include "ltrDateTime.h"
#include "vector"
#include "string"


int main()
{
    DateTime newDate(56002.021);
    double inVector[3];


    inVector[0] = 0.0;
    inVector[1] = 0.0;
    inVector[2] = 0.0;

    cppForm::cxformpp testUnit(newDate, "GSM", inVector);
    double* outVector = testUnit.cxForm("HEEQ");
    double* outVector2 = testUnit.cxForm2("HEEQ");

    std::cout << "Date: " << newDate.getDateTimeString() << std::endl;

    std::cout << "ES cxForm Style: " << std::endl;
    std::cout << "Input Vector:  " << inVector[0] << "," << inVector[1] << "," << inVector[2] << " (GSM)" << std::endl;
    std::cout << "OutPut Vector: " << outVector[0] << "," << outVector[1] << "," << outVector[3] << " (HEEQ)" << std::endl << std::endl;

    cppForm::cxformpp testUnit2(newDate, "HEEQ", outVector);
    outVector = testUnit2.cxForm("GSM");
     outVector2 = testUnit2.cxForm2("GSM");

     std::cout << "Reversal: " << std::endl;
    std::cout << "OutPut Vector: " << outVector[0] << "," << outVector[1] << "," << outVector[3] << " (GSM)" << std::endl << std::endl;



}
