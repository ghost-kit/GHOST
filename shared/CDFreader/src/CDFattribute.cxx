#include "CDFattribute.h"

//==================================================================//
CDFr::CDFattribute::CDFattribute(CDFreader *parent)
{
    this->readerParent = parent;

}

CDFr::CDFattribute::CDFattribute(CDFr::CDFvariable *parent)
{
    this->variableParent = parent;
}

//==================================================================//
CDFr::CDFattribute::~CDFattribute()
{
    this->readerParent = NULL;

}

//==================================================================//
CDFr::CDFreader *CDFr::CDFattribute::getParent()
{

    return this->readerParent;
}

void CDFr::CDFattribute::setParent(CDFr::CDFreader *parent)
{
    this->readerParent = parent;
}

//==================================================================//
int64_t CDFr::CDFattribute::getNumberOfEntries()
{

    return this->Data.size();
}

//==================================================================//
long CDFr::CDFattribute::getType()
{

    return this->type;
}

void CDFr::CDFattribute::setType(long type)
{
    this->type = type;
}

//==================================================================//
bool CDFr::CDFattribute::isGlobalAttr()
{

    return this->global;
}

//==================================================================//
bool CDFr::CDFattribute::isVariableAttr()
{
    return !this->global;
}

//==================================================================//
void CDFr::CDFattribute::setGlobalAtt()
{
    this->global = true;
}

//==================================================================//
void CDFr::CDFattribute::setVariableAtt()
{
    this->global = false;
}

//==================================================================//
QString CDFr::CDFattribute::getAttributeName()
{

    return this->AttName;
}

//==================================================================//
void CDFr::CDFattribute::setAttributeName(QString name)
{
    this->AttName = name;
}

//==================================================================//
QVariant CDFr::CDFattribute::getAttributeItem(int64_t index)
{
    if(this->Data.size() < index)
        return this->Data[index];
    else
        return QVariant(-99999999);
}

//==================================================================//
void CDFr::CDFattribute::addAttributeItem(QVariant item)
{
    this->Data.push_back(item);
}

