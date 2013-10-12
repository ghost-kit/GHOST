#ifndef CDFBADDATAHANDLER_H
#define CDFBADDATAHANDLER_H

#include "CDFreader.h"
#include "CDFattribute.h"
#include "CDFvariable.h"
#include "CDFBadDataHandler.h"
#include "CDFerror.h"

namespace CDFr
{

class CDFattribute;
class CDFreader;
class CDFvariable;
class CDFerrorHandler;

class CDFbadDataHandler
{
public:
    CDFbadDataHandler();
    ~CDFbadDataHandler();
};
}


#endif //CDFBADDATAHANDLER_H