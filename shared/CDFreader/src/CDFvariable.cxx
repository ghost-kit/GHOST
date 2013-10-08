#include "CDFvariable.h"
#include <iostream>

//==================================================================//
CDFr::CDFvariable::CDFvariable(CDFr::CDFreader *parent)
{
    this->parent = parent;
}

//==================================================================//
CDFr::CDFvariable::~CDFvariable()
{
}

//==================================================================//
CDFr::CDFreader *CDFr::CDFvariable::getParent()
{

    return this->parent;
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
long CDFr::CDFvariable::getNumberEntries()
{

    return this->numEntries;
}

//==================================================================//
void CDFr::CDFvariable::setNumberEntries(long numEntries)
{
    this->numEntries = numEntries;
}

//==================================================================//
long CDFr::CDFvariable::getNumberAtts()
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
QList<QVariant> CDFr::CDFvariable::getDataEntry(int64_t index)
{
    QList<QVariant> dataList;
    long numValues = 1;

    //open the file
    CDFid fileID;
    CDFstatus status;
    status = CDFopenCDF(this->parent->getFileName().toAscii().data(), &fileID);

    //check error
    if(this->parent->setErrorStatus(status))
    {
        std::cerr << "Error Opening File" << std::endl;
        return dataList;
    }

    //calculate the number of values
    if(this->getNumberDims() > 0)
    {
        for(int s = 0; s < this->getNumberDims(); s++)
        {
            numValues *= this->getDim(s);
        }
    }

    //data allocation
    void *data = NULL;
    if(!this->allocateRecordMemory(this->VarType, data, numValues))
    {
        std::cerr << "Failed to Allocate Memory." << std::endl;
        return dataList;
    }

    //call the CDF file
    long varNum = CDFgetVarNum(fileID, this->Name.toAscii().data());

    status = CDFgetzVarRecordData(fileID, varNum, index, data);


    //loop through the values
    for(int x = 0; x < numValues; x++)
    {


    }

    //close the file
    CDFcloseCDF(fileID);

    //memmory managment
    if(data)
    {
             if(this->VarType == CDF_FLOAT)       delete [] (float*)data;
        else if(this->VarType == CDF_DOUBLE)      delete [] (double*)data;
        else if(this->VarType == CDF_INT1)        delete [] (int8_t*)data;
        else if(this->VarType == CDF_UINT1)       delete [] (u_int8_t*)data;
        else if(this->VarType == CDF_BYTE)        delete [] (int8_t*)data;
        else if(this->VarType == CDF_INT2)        delete [] (int16_t*)data;
        else if(this->VarType == CDF_UINT2)       delete [] (u_int16_t*)data;
        else if(this->VarType == CDF_INT4)        delete [] (int32_t*)data;
        else if(this->VarType == CDF_UINT4)       delete [] (u_int32_t*)data;
        else if(this->VarType == CDF_INT8)        delete [] (int64_t*)data;
        else if(this->VarType == CDF_FLOAT)       delete [] (char*)data;
        else if(this->VarType == CDF_REAL4)       delete [] (float*)data;
        else if(this->VarType == CDF_REAL8)       delete [] (float*)data;
        else if(this->VarType == CDF_EPOCH)       delete [] (double*)data;
        else if(this->VarType == CDF_EPOCH16)     delete [] (double*)data;
    }


    return dataList;

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
bool CDFr::CDFvariable::attributeExists(QString name)
{
    QStringList keys = this->Attributes.keys();

    return keys.contains(name);
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
void CDFr::CDFvariable::setAttributeList(CDFr::attributeList Attributes)
{
    this->Attributes = Attributes;
}

//==================================================================//
bool CDFr::CDFvariable::allocateRecordMemory(long dataType, void *&data, long numValues)
{
    switch(dataType)
    {
    case CDF_REAL4:
    case CDF_FLOAT:

        data = new float[numValues];
        break;

    case CDF_EPOCH:
    case CDF_REAL8:
    case CDF_DOUBLE:
        data = new double[numValues];
        break;

    case CDF_BYTE:
    case CDF_INT1:
        data = new int8_t[numValues];
        break;

    case CDF_UINT1:
        data = new u_int8_t[numValues];
        break;

    case CDF_INT2:
        data = new int16_t[numValues];
        break;

    case CDF_UINT2:
        data = new u_int16_t[numValues];
        break;

    case CDF_INT4:
        data = new int32_t[numValues];
        break;

    case CDF_UINT4:
        data = new u_int32_t[numValues];
        break;

    case CDF_TIME_TT2000:
    case CDF_INT8:
        data = new int64_t[numValues];
        break;

    case CDF_CHAR:
        data = new char[numValues+1];
        break;

    case CDF_UCHAR:
        data = new uchar[numValues+1];
        break;

    case CDF_EPOCH16:
        data = new double[numValues*2];
        break;

    default:
        return false;

        break;
    }

    return true;
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
