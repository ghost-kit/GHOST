#include "CDFvariable.h"

//==================================================================//
CDFr::CDFvariable::CDFvariable(CDFr::CDFreader *parent)
{
}

//==================================================================//
CDFr::CDFvariable::~CDFvariable()
{
}

//==================================================================//
CDFr::CDFreader *CDFr::CDFvariable::getParent()
{

    return NULL;
}

//==================================================================//
QString CDFr::CDFvariable::getVarName()
{

    return QString("");
}

//==================================================================//
int64_t CDFr::CDFvariable::getVarType()
{

    return 0;
}

//==================================================================//
int64_t CDFr::CDFvariable::getNumberDims()
{

    return 0;
}

//==================================================================//
int64_t CDFr::CDFvariable::getDim(int dim)
{

    return 0;
}

//==================================================================//
int64_t *CDFr::CDFvariable::getAllDims()
{

    return 0;
}

//==================================================================//
int64_t CDFr::CDFvariable::getNumberEntries()
{

    return 0;
}

//==================================================================//
int64_t CDFr::CDFvariable::getNumberAtts()
{

    return 0;
}

//==================================================================//
int64_t CDFr::CDFvariable::getMonotonicState()
{

    return 0;
}

//==================================================================//
bool CDFr::CDFvariable::isZvar()
{

    return false;
}

//==================================================================//
QVariant CDFr::CDFvariable::getFillValue()
{

    return QVariant("");
}

//==================================================================//
QVariant CDFr::CDFvariable::getMinimum()
{

    return QVariant("");
}

//==================================================================//
QVariant CDFr::CDFvariable::getMaximum()
{

    return QVariant("");
}

//==================================================================//
QVariant CDFr::CDFvariable::getDataItem(int64_t index)
{

    return QVariant("");
}

//==================================================================//
CDFr::CDFattribute* CDFr::CDFvariable::getAttribute(int64_t index)
{

    return NULL;
}

//==================================================================//
CDFr::CDFattribute* CDFr::CDFvariable::getAttirbute(QString name)
{

    return NULL;
}

//==================================================================//
int64_t CDFr::CDFvariable::fromXYZ(int64_t x, int64_t y, int64_t z)
{

    return 0;
}

//==================================================================//
int64_t CDFr::CDFvariable::fromXY(int64_t x, int64_t y)
{
    return 0;
}

//==================================================================//
