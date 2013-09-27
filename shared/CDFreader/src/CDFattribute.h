#ifndef CDFATTRIBUTE_H
#define CDFATTRIBUTE_H

#include "CDFreader.h"
#include "CDFvariable.h"
#include "CDFBadDataHandler.h"
#include "CDFerror.h"

#include <QString>
#include <QVariant>
#include <QList>




namespace CDFr
{

class CDFvariable;
class CDFreader;
class CDFbadDataHandler;
class CDFerrorHandler;

class CDFattribute
{
public:

    //constructor/destructor
    CDFattribute(CDFr::CDFreader * parent);
    CDFattribute(CDFr::CDFvariable * parent);

    ~CDFattribute();

    //File Related Information
    CDFreader*  getParent();
    void        setParent(CDFreader* parent);

    //Attribute information
    int64_t           getNumberOfEntries();
    long              getType();
    void              setType(long type);

    bool              isGlobalAttr();
    bool              isVariableAttr();
    void              setGlobalAtt();
    void              setVariableAtt();

    //data
    QString           getAttributeName();
    void              setAttributeName(QString name);

    QVariant          getAttributeItem(int64_t index);
    void              addAttributeItem(QVariant item);

protected:
    //File Variables
    CDFr::CDFreader*  readerParent;
    CDFr::CDFvariable* variableParent;

    //Attribute info
    QString           AttName;
    int64_t           type;
    bool              global;

    //data
    QList<QVariant>   Data;

};

}

#endif //CDFATTRIBUTE_H
