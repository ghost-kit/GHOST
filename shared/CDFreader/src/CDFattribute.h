#ifndef CDFATTRIBUTE_H
#define CDFATTRIBUTE_H

#include "CDFreader.h"
#include "CDFvariable.h"
#include "CDFBadDataHandler.h"

#include <QString>
#include <QVariant>
#include <QList>




namespace CDFr
{

class CDFvariable;
class CDFreader;
class CDFbadDataHandler;

class CDFattribute
{
public:

    //constructor/destructor
    CDFattribute();
    ~CDFattribute();

    //File Related Information
    CDFreader*  getParent();

    //Attribute information
    int64_t           getNumberOfEntries();
    int64_t           getType();

    bool              isGlobal();

    //data
    QString           getAttributeName();
    QVariant          getAttributeItem(int64_t index);

protected:
    //File Variables
    CDFreader*  Parent;

    //Attribute info
    QString           AttName;
    int64_t           type;
    bool              global;

    //data
    QList<QVariant>   Data;
};

}

#endif //CDFATTRIBUTE_H
