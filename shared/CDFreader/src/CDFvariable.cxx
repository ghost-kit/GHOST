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

    return this->Dims.size();
}


//==================================================================//
int64_t CDFr::CDFvariable::getDim(int dim)
{

    if(this->getNumberDims() > 0 && dim < this->getNumberDims())
    {
       return this->Dims[dim];
    }
    else
    {
        return 0;
    }
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
long CDFr::CDFvariable::getNumberRecords()
{

    return this->numberRecords;
}

//==================================================================//
void CDFr::CDFvariable::setNumberRecords(long numEntries)
{
    this->numberRecords = numEntries;
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
QList<QVariant> CDFr::CDFvariable::getDataEntry(long index)
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
    if(this->VarType == CDF_CHAR || this->VarType == CDF_UCHAR)
    {
        for(int x = 0; x < numValues; x++)
        {
            dataList.push_back(this->convertVoid2Variant(data, this->VarType, x));
        }
    }
    else
    {
        for(int x = 0; x < numValues; x++)
        {
            //convert the value to a variant and store
            dataList.push_back(this->convertVoid2Variant(data, this->VarType, x));
        }
    }

    //close the file
    CDFcloseCDF(fileID);

    //memmory managment
    if(data)
    {
        if(this->VarType == CDF_FLOAT)       delete [] (CDFr::float_t*)data;
        else if(this->VarType == CDF_DOUBLE)      delete [] (CDFr::double_t*)data;
        else if(this->VarType == CDF_INT1)        delete [] (CDFr::int1_t*)data;
        else if(this->VarType == CDF_UINT1)       delete [] (CDFr::uint1_t*)data;
        else if(this->VarType == CDF_BYTE)        delete [] (CDFr::byte_t*)data;
        else if(this->VarType == CDF_INT2)        delete [] (CDFr::int2_t*)data;
        else if(this->VarType == CDF_UINT2)       delete [] (CDFr::uint2_t*)data;
        else if(this->VarType == CDF_INT4)        delete [] (CDFr::int4_t*)data;
        else if(this->VarType == CDF_UINT4)       delete [] (CDFr::uint4_t*)data;
        else if(this->VarType == CDF_INT8)        delete [] (CDFr::int8_t*)data;
        else if(this->VarType == CDF_REAL4)       delete [] (CDFr::real4_t*)data;
        else if(this->VarType == CDF_REAL8)       delete [] (CDFr::real8_t*)data;
        else if(this->VarType == CDF_EPOCH)       delete [] (CDFr::epoch_t*)data;
        else if(this->VarType == CDF_EPOCH16)     delete [] (double*)data;
    }


    return dataList;

}

//==================================================================//
QString CDFr::CDFvariable::getDataEntryAsDateString(long index, bool iso)
{
    //get the data entry as a double
    double epoch = this->getDataEntry(index)[0].toDouble();

    //return the double as an EPOCH string
    if(!iso)
    {
        return this->getDateStringFromEPOCH(epoch);
    }
    else
    {
        return this->getIso8601DateStringFromEPOCH(epoch);
    }
}

//==================================================================//
QString CDFr::CDFvariable::getDateStringFromEPOCH(double epoch)
{
    char format[EPOCHx_FORMAT_MAX+1];
    char encoded[EPOCHx_STRING_MAX+1];

    sprintf(format, "<dom.02>-<month>-<year> <hour>:<min>:<sec>.<fos>");
    encodeEPOCHx(epoch, format, encoded);

    return  QString(encoded);
}

//==================================================================//
QString CDFr::CDFvariable::getIso8601DateStringFromEPOCH(double epoch)
{
    char format[EPOCHx_FORMAT_MAX+1];
    char encoded[EPOCHx_STRING_MAX+1];

    sprintf(format, "<year><mm.02><dom.02>T<hour><min><sec>Z");
    encodeEPOCHx(epoch, format, encoded);

    return  QString(encoded);
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
void CDFr::CDFvariable::setElementReadLength(long elementReadLength)
{
    this->elementReadLength = elementReadLength;
}

//==================================================================//
bool CDFr::CDFvariable::allocateRecordMemory(long dataType, void *&data, long numValues)
{
    //this sets the allocation size with the element length
    long allocSize = numValues * this->elementReadLength;

    switch(dataType)
    {
    case CDF_REAL4:
    case CDF_FLOAT:

        data = new float[allocSize];
        break;

    case CDF_EPOCH:
    case CDF_REAL8:
    case CDF_DOUBLE:
        data = new double[allocSize];
        break;

    case CDF_BYTE:
    case CDF_INT1:
        data = new int8_t[allocSize];
        break;

    case CDF_UINT1:
        data = new u_int8_t[allocSize];
        break;

    case CDF_INT2:
        data = new int16_t[allocSize];
        break;

    case CDF_UINT2:
        data = new u_int16_t[allocSize];
        break;

    case CDF_INT4:
        data = new int32_t[allocSize];
        break;

    case CDF_UINT4:
        data = new u_int32_t[allocSize];
        break;

    case CDF_TIME_TT2000:
    case CDF_INT8:
        data = new int64_t[allocSize];
        break;

    case CDF_CHAR:
        data = new char[allocSize+1];
        break;

    case CDF_UCHAR:
        data = new uchar[allocSize+1];
        break;

    case CDF_EPOCH16:
        data = new double[allocSize*2];
        break;

    default:
        return false;

        break;
    }

    return true;
}

//==================================================================//
QVariant CDFr::CDFvariable::convertVoid2Variant(const void *data, const long dataType, const long index)
{
    //build variant
    QVariant returnVal;

    switch(dataType)
    {
    case CDF_REAL4:
    case CDF_FLOAT:
    {
        returnVal = QVariant(((float*)data)[index]);
        break;
    }
    case CDF_EPOCH:
    case CDF_REAL8:
    case CDF_DOUBLE:
    {
        returnVal = QVariant(((double*)data)[index]);
        break;
    }
    case CDF_BYTE:
    case CDF_INT1:
    {
        returnVal = QVariant(((int8_t*)data)[index]);
        break;
    }
    case CDF_UINT1:
    {
        returnVal = QVariant(((u_int8_t*)data)[index]);
        break;
    }
    case CDF_INT2:
    {
        returnVal = QVariant(((int16_t*)data)[index]);
        break;
    }
    case CDF_UINT2:
    {
        returnVal = QVariant(((u_int16_t*)data)[index]);
        break;
    }
    case CDF_INT4:
    {
        returnVal = QVariant(((int32_t*)data)[index]);
        break;
    }
    case CDF_UINT4:
    {
        returnVal = QVariant(((u_int32_t*)data)[index]);
        break;
    }
    case CDF_TIME_TT2000:
    case CDF_INT8:
    {
        returnVal = QVariant(((int64_t*)data)[index]);
        break;
    }
    case CDF_CHAR:
    {
        char *dataConv = (char*) data;
        long start = index*this->elementReadLength;

        QString dataValue = QString(dataConv);
        dataValue = dataValue.mid(start, this->elementReadLength);
        dataValue = dataValue.trimmed();

        returnVal = QVariant(dataValue);

        break;
    }
    case CDF_UCHAR:
    {
        uchar *dataConv = (uchar*) data;
        long start = index*this->elementReadLength;

        QString dataValue = QString((char*)dataConv);
        dataValue = dataValue.mid(start, this->elementReadLength);
        dataValue = dataValue.trimmed();

        returnVal = QVariant(dataValue);
        break;
    }
    case CDF_EPOCH16:
    {
        QPair<double, double> epoch16;

        std::cerr << "CDF_EPOCH16 not yet supported... first half being returned" << std::endl;

        //TODO: implement this CDF_EPOCH16 data type
        returnVal = QVariant(((double *)data)[index]);
        break;
    }
    default:
        break;
    }

    return returnVal;

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
