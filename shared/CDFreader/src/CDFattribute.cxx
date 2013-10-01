#include "CDFattribute.h"
#include <iostream>

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
    delete [] this->Data;

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

    return this->Data->size();
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

    if(this->Data)
    {
        return this->Data->operator [](index);
    }
    else
    {
        //make it a NaN if we don't know what to do
        return QVariant(0.0/0.0);
    }
}

//==================================================================//
void CDFr::CDFattribute::addAttributeItem(QVariant item)
{
    this->Data->push_back(item);
}

void CDFr::CDFattribute::setAttributeList(QList<QVariant> *list)
{
    this->Data = list;
}

