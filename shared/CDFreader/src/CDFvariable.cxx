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

    return this->Name;
}

//==================================================================//
void CDFr::CDFvariable::setVarName(QString name)
{
    this->Name = name;
}

//==================================================================//
long CDFr::CDFvariable::getVarType()
{

    return this->VarType;
}

void CDFr::CDFvariable::setVarType(long type)
{
    this->VarType = type;
}

//==================================================================//
int64_t CDFr::CDFvariable::getNumberDims()
{

    return this->Dims.count();
}


//==================================================================//
int64_t CDFr::CDFvariable::getDim(int dim)
{

    return this->Dims[dim];
}

void CDFr::CDFvariable::setDim(long index, int dimValue)
{
    this->Dims[index] = dimValue;
}

//==================================================================//
int64_t *CDFr::CDFvariable::getAllDims()
{

    int64_t *dims = new int64_t[this->getNumberDims()];
    for(int x = 0; x < this->getNumberDims(); x++)
    {
        dims[x] = this->Dims[x];
    }

    return dims;
}

void CDFr::CDFvariable::setAllDims(long dims[], int count)
{

    for(int x = 0; x < count; x++)
    {
        this->Dims[x] = dims[x];
    }
}

//==================================================================//
int64_t CDFr::CDFvariable::getNumberEntries()
{

    return this->numEntries;
}

//==================================================================//
void CDFr::CDFvariable::setNumberEntries(long numEntries)
{
    this->numEntries = numEntries;
}

//==================================================================//
int64_t CDFr::CDFvariable::getNumberAtts()
{

    return this->Attributes.count();
}

//==================================================================//
void CDFr::CDFvariable::addAttribute(CDFr::CDFattribute *attribute)
{
    this->Attributes[attribute->getAttributeName()] = attribute;
}

//==================================================================//
int64_t CDFr::CDFvariable::getMonotonicState()
{

    return this->monotonicState;
}

//==================================================================//
void CDFr::CDFvariable::setMonotonicState(long state)
{
    this->monotonicState = state;
}


//==================================================================//
QVariant CDFr::CDFvariable::getFillValue()
{

    return this->fillValue;
}

//==================================================================//
void CDFr::CDFvariable::setFillValue(QVariant fill)
{
    this->fillValue = fill;
}

//==================================================================//
QVariant CDFr::CDFvariable::getMinimum()
{

    return this->minimum;
}

//==================================================================//
void CDFr::CDFvariable::setMinimum(QVariant minimum)
{
    this->minimum = minimum;
}

//==================================================================//
QVariant CDFr::CDFvariable::getMaximum()
{

    return this->maximum;
}

//==================================================================//
void CDFr::CDFvariable::setMaximum(QVariant maximum)
{
    this->maximum = maximum;
}

//==================================================================//
QVariant CDFr::CDFvariable::getDataItem(int64_t index)
{

    return QVariant("");
}

//==================================================================//
CDFr::CDFattribute* CDFr::CDFvariable::getAttribute(int64_t index)
{

    QStringList keys = this->Attributes.keys();

    return this->Attributes[keys[index]];
}

//==================================================================//
CDFr::CDFattribute* CDFr::CDFvariable::getAttirbute(QString name)
{

    if(this->Attributes.contains(name))
    {
        return this->Attributes[name];
    }
    else
    {
        return NULL;
    }
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

bool CDFr::CDFvariable::getRecordVaries() const
{
    return this->recordVaries;
}

//==================================================================//
void CDFr::CDFvariable::setRecordVaries(bool value)
{
    this->recordVaries = value;
}

//==================================================================//
void CDFr::CDFvariable::setDimVaries(const long value[], long count)
{

    for(int x = 0; x < count; x++)
    {
        this->dimVaries.push_back(value[x]);
    }
}

//==================================================================//
long CDFr::CDFvariable::getDimVaries(long index) const
{

    return this->dimVaries[index];
}
