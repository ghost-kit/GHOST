#ifndef CDFERROR_H
#define CDFERROR_H

#include "QString"
#include "QStringList"
#include "QMap"
#include <iomanip>

#include "CDFerrorDefs.h"

namespace CDFr
{
class CDFerrorHandler
{
public:
    CDFerrorHandler();
    ~CDFerrorHandler();

    //error related methods
    long         getError();
    void         setError(long errorCode);
    QString      getErrorString();
    bool         hasError();

protected:

    //internal error code
    long         errorCode;

private:

    //internal error lookup table
    QMap<int, QString> errorTable;
    void populateErrorTable();


};
}

#endif // CDFERROR_H
