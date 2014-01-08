#include "cppxform.h"
#include "iostream"
#include "ltrDateTime.h"
#include "vector"
#include "string"


int main()
{
    DateTime newDate(56002.021);
    std::vector<double> inVector;


    inVector.push_back(-896921337.28302002);
    inVector.push_back(220296912.43620300);
    inVector.push_back(44419205.01961136);

    cppForm::cxformpp testUnit(newDate, "GSM", inVector);
    double* outVector = testUnit.cxForm("HEEQ");
    double* outVector2 = testUnit.cxForm2("HEEQ");

    std::cout << "Date: " << newDate.getDateTimeString() << std::endl;

    std::cout << "ES cxForm Style: " << std::endl;
    std::cout << "Input Vector:  " << inVector[0] << "," << inVector[1] << "," << inVector[2] << " (GSM)" << std::endl;
    std::cout << "OutPut Vector: " << outVector[0] << "," << outVector[1] << "," << outVector[3] << " (HEEQ)" << std::endl << std::endl;

    std::cout << "ES my Style: " << std::endl;
    std::cout << "Input Vector:  " << inVector[0] << "," << inVector[1] << "," << inVector[2] << " (GSM)" << std::endl;
    std::cout << "OutPut Vector: " << outVector[0] << "," << outVector[1] << "," << outVector[3] << " (HEEQ)" << std::endl;

}
