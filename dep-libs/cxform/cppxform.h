#ifndef CXFORMPP_H
#define CXFORMPP_H

#include "cxform.h"
#include "ltrDateTime.h"
#include "math.h"
#include "string"
#include "vector"


namespace cppForm
{

class cppxform
{
public:
    //constructors/destructors
    cppxform(DateTime Date, const char* sourceSystem, double inputVector[]);
    ~cppxform();

    //transform commands
    double* cxForm(const char* destSystem);

    //get MJD
    double getMJD();

    //set vector
    void setInVector(double vec[]);

    //set source system
    void setSourceSystem(const char *name);

    //clean handler
    void cleanHandler();

protected:
    long   cxRound(double d);
    long   getES(int version=0);

private:
    double               inVector[3];
    DateTime             inDate;
    std::string          inSystem;
    double*              outVector;

    std::vector<double*> outVectorHandler;

};

}

#endif // CXFORMPP_H
