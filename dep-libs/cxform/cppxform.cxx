#include "cppxform.h"

namespace cppForm
{

cppxform::cppxform(DateTime Date, const char* sourceSystem, double inputVector[])
{
    this->inVector[0] = inputVector[0];
    this->inVector[1] = inputVector[1];
    this->inVector[2] = inputVector[2];

    this->inDate = Date;
    this->inSystem = std::string(sourceSystem);
    this->outVector = NULL;

}

cppxform::~cppxform()
{
    if(this->outVector) delete [] this->outVector;
    this->cleanHandler();
}

//USER SHOULD NOT DELETE THE RETURNED VALUE
// This class manages the memory dealocation.
// User should just set the value to void.
double *cppxform::cxForm(const char *destSystem)
{
    if(this->outVector)
    {
        this->outVectorHandler.push_back(this->outVector);
        this->outVector = NULL;
    }

    this->outVector = new double[3];

    long es = this->getES();

    int retVal = cxform(this->inSystem.c_str(), destSystem, (double)es, this->inVector, this->outVector);

//    std::cerr << "inVector:  " << this->inVector[0] << "," << this->inVector[1] << "," << this->inVector[2] << std::endl;
//    std::cerr << "OutVector: " << this->outVector[0] << "," << this->outVector[1] << "," << this->outVector[2] << std::endl;

    return this->outVector;
}


double cppxform::getMJD()
{
    return this->inDate.getMJD();
}

void cppxform::setInVector(double vec[])
{
    this->inVector[0] = vec[0];
    this->inVector[1] = vec[1];
    this->inVector[2] = vec[2];
}

void cppxform::setSourceSystem(const char *name)
{
    this->inSystem = std::string(name);
}

//if you have cleared all references to the output values,
//  you can use this to clear the memory
void cppxform::cleanHandler()
{
    int numHandles = this->outVectorHandler.size();
    for(int x = 0; x < numHandles; x++)
    {
        delete [] this->outVectorHandler[x];
    }

    this->outVectorHandler.clear();
}

long cppxform::cxRound(double d)
{
    if (d < 0.0)  return (long)(d - 0.5);
    else          return (long)(d + 0.5);
}

long cppxform::getES( int version)
{
    //jd = mjd + 2400000.5
    //es = (jd - 2451545) * 86400;
    long es;

    if(version)
    {
        es = (long)((this->inDate.getMJD()+2400000.5)-2451545) * 86400;
    }
    else
    {
        es = date2es(this->inDate.getYear(), this->inDate.getMonth(),  this->inDate.getDay(),
                     this->inDate.getHour(), this->inDate.getMinute(), this->inDate.getSeconds());
    }

    return es;

}


}
