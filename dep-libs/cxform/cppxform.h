#ifndef CXFORMPP_H
#define CXFORMPP_H

#include "cxform.h"
#include "ltrDateTime.h"
#include "math.h"
#include "string"
#include "vector"


namespace cppForm
{

class cxformpp
{
public:
    //constructors/destructors
    cxformpp(DateTime Date, const char* sourceSystem, double inputVector[]);
    ~cxformpp();

    //transform commands
    double* cxForm(const char* destSystem);
    double* cxForm2(const char *destSystem);

    //get MJD
    double getMJD();

protected:
    long   cxRound(double d);
    long   getES(int version=0);

private:
    std::vector<double>  inVector;
    DateTime             inDate;
    std::string          inSystem;

};

}

#endif // CXFORMPP_H
