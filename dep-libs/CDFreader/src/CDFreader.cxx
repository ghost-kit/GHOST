#include "CDFreader.h"
#include "cdf.h"
#include <iostream>


//==================================================================//
QString CDFr::CDFreader::getVarName(int index)
{

    QStringList keys = this->Variables.keys();

    if(keys.size() < index)
        return keys[index];
    else return QString("Invalid Variable");
}

//==================================================================//
QStringList CDFr::CDFreader::getVarNames()
{

    return QStringList();
}

//==================================================================//
long CDFr::CDFreader::getNumberVariables()
{

    return this->Variables.size();
}

//==================================================================//
CDFr::CDFvariable* CDFr::CDFreader::getVariable(int index)
{

    QStringList keys = this->Variables.keys();
    return this->Variables[keys[index]];
}

//==================================================================//
CDFr::CDFvariable *CDFr::CDFreader::getVariable(QString name)
{

    return this->Variables[name];
}

//==================================================================//
QString CDFr::CDFreader::getGlobalAttName(int index)
{
    QStringList keys = this->Attributes.keys();

    return keys[index];
}

//==================================================================//
QStringList CDFr::CDFreader::getGlobalAttNames()
{
    QStringList returnList;

    return returnList;
}

//==================================================================//
long CDFr::CDFreader::getNumberGlobalAttributes()
{
    return this->Attributes.size();
}

//==================================================================//
CDFr::CDFattribute* CDFr::CDFreader::getGlobalAttribute(int index)
{

    return NULL;
}

//==================================================================//
CDFr::CDFattribute* CDFr::CDFreader::getGlobalAttribute(QString name)
{

    return NULL;
}

//==================================================================//
QString CDFr::CDFreader::getFileName()
{
    return this->FileName;
}

//==================================================================//
long CDFr::CDFreader::getCompressionType()
{

    return this->CompressionType;
}

//==================================================================//
long CDFr::CDFreader::getChecksum()
{

    return this->Checksum;

}

//==================================================================//
long CDFr::CDFreader::getFormat()
{
    return this->Format;
}

//==================================================================//
long CDFr::CDFreader::getEncoding()
{

    return this->Encoding;

}

//==================================================================//
QVariant CDFr::CDFreader::getCdfVersion()
{

    return QVariant("getCdfVersion UNIMPLIMENTED");

}

//==================================================================//
bool CDFr::CDFreader::isRowMajor()
{

    return false;
}

//==================================================================//
bool CDFr::CDFreader::attributeExists(QString name)
{

    if(this->Attributes.contains(name)) return true;
    else return false;

}

//==================================================================//
bool CDFr::CDFreader::variableExists(QString name)
{

    if(this->Variables.contains(name)) return true;
    else return false;

}

//==================================================================//
bool CDFr::CDFreader::hasError()
{

    //get the internal error code
    return this->errorTracker->getError();
}

//==================================================================//
QString CDFr::CDFreader::getErrorString()
{

    //get the internal error string
    return this->errorTracker->getErrorString();
}

//==================================================================//
CDFr::CDFreader::CDFreader(QString FileName)
{
    //variables we need to keep
    long numzVars;
    long numAttrs;
    long maxzRec;

    CDFid fileID;

    //start the error tracker
    this->errorTracker = new CDFr::CDFerrorHandler;

    //Record the File Name
    this->FileName = FileName;

    //Open the File
    CDFstatus status = CDFopenCDF(this->FileName.toAscii().data(), &fileID);

    //std::cerr << "FileName: " << this->FileName.toAscii().data() << std::endl;

    //set error status
    if(this->setErrorStatus(status)) return;

    //set file zMode to 2
    status = CDFsetzMode(fileID, zMODEon2);

    //set zmode error status
    if(this->setErrorStatus(status)) return;

    //variables we can dump (this block will dump the vars not needed after it closes)
    // we don't need rVariable related information, because we are using zMode2.
    {
        long numDims;
        long dimSizes[CDF_MAX_DIMS];
        long maxrRec;
        long numrVars;


        //get the file information
        status = CDFinquireCDF(fileID, &numDims, dimSizes, &this->Encoding, &this->majority, &maxrRec, &numrVars, &maxzRec, &numzVars, &numAttrs);

        //check error status
        if(this->setErrorStatus(status)) return;

    }

    //get the CDF version
    status = CDFgetVersion(fileID, &this->cdfVersion, &this->cdfRelease, &this->cdfIncrement);

    //check error status
    if(this->setErrorStatus(status)) return;

    //get the CDF Format
    status = CDFgetFormat(fileID, &this->Format);

    //check error status
    if(this->setErrorStatus(status)) return;

    //get copyright notice
    {
        char Copyright[CDF_COPYRIGHT_LEN+1];
        status = CDFgetCopyright(fileID, Copyright);

        //check error status
        if(this->setErrorStatus(status)) return;

        //if no error, place notice
        this->Copyright = QString(Copyright);

    }

    //get the checksum for the file
    status = CDFgetChecksum(fileID, &this->Checksum);

    //check for errors
    if(this->setErrorStatus(status)) return;

    //get compression type (issolated block)
    {
        long compressionType;
        long compressionParms[CDF_MAX_PARMS];
        long compressionPercentage;

        //get the compression parameters
        status = CDFgetCompression(fileID, &compressionType, compressionParms, &compressionPercentage);

        //get error status
        if(this->setErrorStatus(status)) return;

        //save the compression information
        this->CompressionType = compressionType;
        for(int x = 0; x < CDF_MAX_PARMS; x++)
        {
            //save the paramaters
            this->CompressionParms.push_back(compressionParms[x]);
            this->CompressionPercentage = compressionPercentage;
        }
    }

    //All of the meta-data has been read.. now we need to get the Variables and Attributes
    //getting zvariable information
    {
        char    varName[CDF_VAR_NAME_LEN256+1];
        long    dataType;
        long    numElements;
        long    recVary;
        long    numDims;
        long    dimSizes[CDF_MAX_DIMS];
        long    dimVarys[CDF_MAX_DIMS];
        long    maxRecordNumber;

        //cycle through all variables
        for(int x = 0; x < numzVars; x++)
        {
            //get information for currect var
            status = CDFinquirezVar(fileID, x, varName, &dataType, &numElements, &numDims, dimSizes, &recVary, dimVarys);

            //check error
            if(this->setErrorStatus(status)) return;

            //get number of records
            status = CDFgetzVarMaxWrittenRecNum(fileID, x, &maxRecordNumber);

            //create the variable
            CDFr::CDFvariable *newVar = new CDFr::CDFvariable(this);

            //populate info from inquirezVar
            newVar->setAllDims(dimSizes, numDims);
            newVar->setVarType(dataType);
            newVar->setVarName(QString(varName));
            newVar->setDimVaries(dimVarys, numDims);
            newVar->setRecordVaries(recVary);
            newVar->setNumberRecords(maxRecordNumber+1);  //we want the number of records, not the max number, so add 1
            newVar->setElementReadLength(numElements);

            //populate attributes
            newVar->setAttributeList(this->processAttributesList(fileID, CDFr_VARIABLE_SCOPE, x, newVar));

            if(newVar->attributeExists("FILLVAL"))
            {
                QVariant fillVal = newVar->getAttirbute("FILLVAL")->getAttributeItem(0);
                newVar->setFillValue(fillVal);
            }
            else
            {
                //make sure we get a viewable type for bad fill values
                if(newVar->getVarType() == CDF_UCHAR || newVar->getVarType() == CDF_CHAR)
                {
                    newVar->setFillValue(QString("N/A"));
                }
                else
                {
                    newVar->setFillValue(0.0);
                }
            }

            //add the variable to the list
            this->Variables[QString(varName)] = newVar;
        }
    }

    //get global attributes
    this->Attributes = this->processAttributesList(fileID, CDFr_GLOBAL_SCOPE);

    //close the file
    CDFcloseCDF(fileID);

}



//==================================================================//
CDFr::CDFreader::~CDFreader()
{
}

//==================================================================//
//helpers//

//sets error status and returns true for error, false for none.
bool CDFr::CDFreader::setErrorStatus(CDFstatus status)
{
    if(status != CDF_OK)
    {
        //foward the error status
        this->errorTracker->setError(status);
        std::cerr << "ERROR: " << this->getErrorString().toStdString() << std::endl;
        return true;
    }
    else
    {
        return false;
    }
}
QString CDFr::CDFreader::getCopyright() const
{
    return Copyright;
}

void CDFr::CDFreader::setCopyright(const QString &value)
{
    Copyright = value;
}
long CDFr::CDFreader::getMajority() const
{
    return majority;
}

void CDFr::CDFreader::setMajority(long value)
{
    majority = value;
}

QString CDFr::CDFreader::getDateStringFromEPOCH(double epoch)
{
    return CDFvariable::getDateStringFromEPOCH(epoch);
}

QString CDFr::CDFreader::getIso8601DateStringFromEPOCH(double epoch)
{
    return CDFvariable::getIso8601DateStringFromEPOCH(epoch);
}



//==================================================================//
void CDFr::CDFreader::clearErrorStatus()
{
    this->errorTracker->setError(CDFr_OK);
}

//==================================================================//
CDFr::attributeList CDFr::CDFreader::processAttributesList(CDFid fileID, long Scope, long VarNum, CDFr::CDFvariable * variable)
{
    //Generate Attributes
    CDFr::attributeList AttList;
    CDFstatus status;

    long attrScope = 0;
    long maxgEntry = 0;
    long maxzEntry = 0;
    long maxrEntry = 0;
    char attName[CDF_ATTR_NAME_LEN256+1];

    long numElements;
    long dataType;
    long numAttrs;

    //need to get the number of attributes
    status = CDFgetNumAttributes(fileID, &numAttrs);
    if(this->setErrorStatus(status)) return AttList;

    for(int att = 0; att < numAttrs; att++)
    {
        //process the attributes in order
        status = CDFinquireAttr(fileID, att, attName, &attrScope, &maxgEntry, &maxrEntry, &maxzEntry);

        //skip the variable if there is a problem
        if(this->setErrorStatus(status)) { this->clearErrorStatus(); continue;}

        //if the variable isn't the correct scope, move on to the next one.
        if(attrScope != Scope) continue;

        //process differently based on scope
        if(Scope == CDFr_GLOBAL_SCOPE)
        {
            //allocate the new attribute
            CDFr::CDFattribute *newAttr = new CDFr::CDFattribute(this);

            //get required information
            for(int entry = 0; entry <= maxgEntry; entry++)
            {
                status = CDFinquireAttrgEntry(fileID, att, entry, &dataType, &numElements);

                //move on to next entry if this one doesn't exist
                if(status == CDFr_NO_SUCH_ENTRY) continue;

                if(status != CDFr_OK )
                {
                    this->setErrorStatus(status);
                    break;
                }

                newAttr->setAttributeName(QString(attName));
                newAttr->setGlobalAtt();
                newAttr->setType(dataType);

                newAttr->setAttributeList(this->extractAttributeElements(fileID, att, entry, CDFr_GLOBAL_SCOPE));

                AttList[QString(attName)] = newAttr;
            }


        }
        else    // we Variable scope
        {
            //make sure the parent is defined
            if(variable == NULL)
            {
                this->setErrorStatus(CDFr_NO_PARENT_DEFINED);
                break;
            }

            //allocate new attribute
            CDFr::CDFattribute *newAttr = new CDFr::CDFattribute(variable);

            //get attribute information
            status = CDFinquireAttrzEntry(fileID, att, VarNum, &dataType, &numElements);

            //if the entry does not exist, skip it
            if(status == CDFr_NO_SUCH_ENTRY) continue;

            if(status != CDFr_OK)
            {
                this->setErrorStatus(status);
                break;
            }

            newAttr->setAttributeName(QString(attName));
            newAttr->setVariableAtt();
            newAttr->setType(dataType);

            newAttr->setAttributeList(this->extractAttributeElements(fileID, att, VarNum, CDFr_VARIABLE_SCOPE));


            AttList[QString(attName)] = newAttr;

        }

    }


    //return the list
    return AttList;
}


//==================================================================//
QList<QVariant>* CDFr::CDFreader::extractAttributeElements(CDFid fileID, long attrN, long entryNum, long scope)
{
    //the data list...
    QList<QVariant> *dataList = new QList<QVariant>;
    CDFstatus status;

    //std::cerr << "====================" << std::endl;

    char nameOfatt[CDF_ATTR_NAME_LEN256+1];
    status = CDFgetAttrName(fileID, attrN, nameOfatt);


    //get required information
    long dataType;
    long numElements;
    long gzrEntry = 0;

    //set the correct scope for the attribute reader
    if(scope == CDFr_GLOBAL_SCOPE) gzrEntry = 1;
    else gzrEntry = 3;

    status = CDFinquireAttrEntry(fileID, gzrEntry, attrN, entryNum, &dataType, &numElements);

    //if error, set it, and return empty list
    if(this->setErrorStatus(status)) return dataList;

    //get the data
    switch(dataType)
    {
    //1 byte signed integer
    case CDF_CHAR:
    {

        //this is a string element
        //Data is a stirng
        char * data = new char[numElements + 1];

        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        if(this->setErrorStatus(status)) break;

        data[numElements] = '\0';

        //add to record
        QVariant attr = QString(data);

        dataList->push_back(attr);

        break;
    }

    case CDF_BYTE:
    case CDF_INT1:
    {
        //std::cerr << "1 Byte Processing" << std::endl;

        //get the data
        int8_t *data = new int8_t[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((int8_t)data[elem]));
        }

        break;
    }
        //1 byte unsigned integer
    case CDF_UCHAR:
    case CDF_UINT1:
    {
       // std::cerr << "Unsigned 1 Byte Processing" << std::endl;

        //get the data
        uint8_t *data = new uint8_t[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((uint8_t)data[elem]));
        }


        break;
    }
        //2 byte signed integer
    case CDF_INT2:
    {
        //std::cerr << "2 Byte Signed Processing" << std::endl;

        //get the data
        int16_t *data = new int16_t[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((int16_t)data[elem]));
        }

        break;
    }

        //2 byte unsigned integer
    case CDF_UINT2:
    {
       // std::cerr << "2 byte unsigned processing" << std::endl;

        //get the data
        uint16_t *data = new uint16_t[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((uint16_t)data[elem]));
        }
        break;
    }
        //4 byte signed integer
    case CDF_INT4:
    {
       // std::cerr << "4 byte int processing " << std::endl;

        //get the data
        int32_t *data = new int32_t[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((int32_t)data[elem]));
        }
        break;
    }
        //4 byte unsigned integer
    case CDF_UINT4:
    {
       // std::cout << "4 byte unsinged processing" << std::endl;

        //get the data
        uint32_t *data = new uint32_t[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((uint32_t)data[elem]));
        }
        break;
    }
        //8-byte signed integer
    case CDF_INT8:
    case CDF_TIME_TT2000:
    {
       // std::cerr << "8 byte int processing" << std::endl;

        //get the data
        int64_t *data = new int64_t[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((int64_t)data[elem]));
        }
        break;
    }
        //4-byte floating point
    case CDF_REAL4:
    case CDF_FLOAT:
    {
       // std::cerr << "Float Processing" << std::endl;

        //get the data
        float *data = new float[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((float)data[elem]));
        }
        break;
    }
        //8-byte floating point
    case CDF_REAL8:
    case CDF_DOUBLE:
    case CDF_EPOCH:
    {
      ///  std::cerr << "Double Processing" << std::endl;

        //get the data
        double *data = new double[numElements];
        status = CDFgetAttrEntry(fileID, gzrEntry, attrN, entryNum, data);

        //check error - if so, exit the switch
        if(this->setErrorStatus(status)) break;

        //process the data
        for(int elem = 0; elem < numElements; elem++)
        {
            dataList->push_back(QVariant((double)data[elem]));
        }
        break;
    }
        //2 - 8-byte floating points
    case CDF_EPOCH16:
    {

        std::cerr << "CDF_EPOCH16 unimplemented" << std::endl;
        break;
    }

    default:
        std::cerr << "UNKNOWN CDF TYPE" << std::endl;
        break;
    }

    return dataList;
}




