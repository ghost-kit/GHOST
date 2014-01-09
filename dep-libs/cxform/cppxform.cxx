#include "cppxform.h"

namespace cppForm
{

cxformpp::cxformpp(DateTime Date, const char* sourceSystem, double inputVector[])
{
    std::vector<double> newVector;
    newVector.push_back(inputVector[0]);
    newVector.push_back(inputVector[1]);
    newVector.push_back(inputVector[2]);

    this->inVector = newVector;
    this->inDate = Date;
    this->inSystem = std::string(sourceSystem);
}

cxformpp::~cxformpp()
{

}

double *cxformpp::cxForm(const char *destSystem)
{
    long es = this->getES();

    double* newVector = new double[3];
    double* outVector = new double[3];

    newVector[0] = this->inVector[0];
    newVector[1] = this->inVector[1];
    newVector[2] = this->inVector[2];

    int retVal = cxform(this->inSystem.c_str(), destSystem, (double)es, newVector, outVector);

    return outVector;
}

double *cxformpp::cxForm2(const char *destSystem)
{
    long es = this->getES(true);

    double* newVector = new double[3];
    double* outVector = new double[3];

    newVector[0] = this->inVector[0];
    newVector[1] = this->inVector[1];
    newVector[2] = this->inVector[2];

    int retVal = cxform(this->inSystem.c_str(), destSystem, (double)es, newVector, outVector);

    return outVector;
}

double cxformpp::getMJD()
{
    return this->inDate.getMJD();
}

long cxformpp::cxRound(double d)
{
    if (d < 0.0)  return (long)(d - 0.5);
    else          return (long)(d + 0.5);
}

long cxformpp::getES( int version)
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
