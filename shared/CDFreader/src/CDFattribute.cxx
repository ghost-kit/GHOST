#include "CDFattribute.h"

//==================================================================//
CDFr::CDFattribute::CDFattribute()
{

}

//==================================================================//
CDFr::CDFattribute::~CDFattribute()
{
}

//==================================================================//
CDFr::CDFreader *CDFr::CDFattribute::getParent()
{

    return NULL;
}

//==================================================================//
int64_t CDFr::CDFattribute::getNumberOfEntries()
{

    return 0;
}

//==================================================================//
int64_t CDFr::CDFattribute::getType()
{

    return 0;
}

//==================================================================//
bool CDFr::CDFattribute::isGlobal()
{

    return false;
}

//==================================================================//
QString CDFr::CDFattribute::getAttributeName()
{

    return QString("");
}

//==================================================================//
QVariant CDFr::CDFattribute::getAttributeItem(int64_t index)
{
    return QVariant("");
}

