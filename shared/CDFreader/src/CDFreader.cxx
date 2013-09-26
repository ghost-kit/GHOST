#include "CDFreader.h"
#include "cdf.h"
#include <iostream>


//==================================================================//
QString CDFr::CDFreader::getVarName(int index)
{

    return QString("getVarName UNIMPLEMENTED");
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

    return QString("getGlobalAttName UNIMPLEMENTED");
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
    return QString("getFileName UNIMPLEMENTED");
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
void CDFr::CDFreader::setBadDataHandler(CDFbadDataHandler *handler)
{

    this->BadDataHandler = handler;
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
    CDFid fileID;

    //start the error tracker
    this->errorTracker = new CDFr::CDFerrorHandler;

    //Record the File Name
    this->FileName = FileName;

    //Open the File
    CDFstatus status = CDFopenCDF(this->FileName.toAscii().data(), &fileID);

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
        long maxzRec;


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

        //cycle through all variables
        for(int x = 0; x < numzVars; x++)
        {
            //get information for currect var
            status = CDFinquirezVar(fileID, x, varName, &dataType, &numElements, &numDims, dimSizes, &recVary, dimVarys);

            //check error
            if(this->setErrorStatus(status)) return;

            //create the variable
            CDFr::CDFvariable *newVar = new CDFr::CDFvariable(this);

            //populate info from inquirezVar
            newVar->setAllDims(dimSizes, numDims);
            newVar->setVarType(dataType);
            newVar->setVarName(QString(varName));
            newVar->setDimVaries(dimVarys, numDims);
            newVar->setRecordVaries(recVary);
            newVar->setNumberEntries(numElements);

            //populate attributes
            newVar->setAttributeList(this->processAttributesList(fileID, CDFr_VARIABLE_SCOPE, x));


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

//==================================================================//
void CDFr::CDFreader::clearErrorStatus()
{
    this->errorTracker->setError(CDFr_OK);
}

//==================================================================//
CDFr::attributeList CDFr::CDFreader::processAttributesList(CDFid fileID, long Scope, long VarNum)
{
    //Generate Attributes
    CDFr::attributeList AttList;


    //getting global attributes
    CDFstatus status;

    bool global;
    if(Scope == CDFr_GLOBAL_SCOPE)
    {
        global = true;
    }
    else
    {
        global = false;
    }

    long numAttrs;
    long entryN;
    long attrN;
    long attrScope;
    long maxEntry;
    long dataType;
    long numElems;
    char attrName[CDF_ATTR_NAME_LEN256+1];


    status = CDFgetNumAttributes(fileID, &numAttrs);

    //check for error status
    if(this->setErrorStatus(status)) return AttList;

    //cycle through the attributes
    for(long x = 0; x < numAttrs; x ++)
    {
        status = CDFattrInquire(fileID, x, attrName, &attrScope, &maxEntry);
        if(this->setErrorStatus(status)) return AttList;

        //we only want global scope attributes
        if(global)
        {
            //make sure we have the correct scope
            if(attrScope != CDFr_GLOBAL_SCOPE) continue;

            //get all entries for the scope
            for(entryN=0; entryN <= maxEntry; entryN++)
            {
                status = CDFinquireAttrgEntry(fileID, attrN, entryN, &dataType, &numElems);

                //No Such entry is an expected error
                if(status != CDFr_NO_SUCH_ENTRY && status != CDFr_NO_SUCH_ATTR && this->setErrorStatus(status)) break;
                else this->clearErrorStatus();

                CDFr::CDFattribute *newAttr = new CDFr::CDFattribute(this);

                //add relevent data
                newAttr->setAttributeName(QString(attrName));
                newAttr->setGlobalAtt();
                newAttr->setType(dataType);

                //TODO: process the actual attribute
                //                newAttr->addAttributeItem();


                //add to the list
                AttList[QString(attrName)] = newAttr;

            }

        }
        else
        {
            //make sure we have the correct scope
            if(attrScope != CDFr_VARIABLE_SCOPE) continue;

            //status = CDFinquireAttrzEntry(fileID, attrN, VarNum, &dataType, &numElems);
            if(this->setErrorStatus(status)) return AttList;


            CDFr::CDFattribute *newAttr = new CDFr::CDFattribute(this);

            //add relevent data
            newAttr->setAttributeName(QString(attrName));
            newAttr->setGlobalAtt();
            newAttr->setType(dataType);

            //TODO: process the actual attribute
            //            newAttr->addAttributeItem();


            //add to the list
            AttList[QString(attrName)] = newAttr;


        }

        //get the stuff


    }

    return AttList;
}

////==================================================================//
//void CDFr::CDFreader::processAttributeData(void* parent, bool global, CDFattribute *attribute, const CDFid file, const long attrN, const long entryN, const long dataType)
//{

//    //hack for parent type. TODO: Change this to a polymorphic class exchange
//    CDFr::CDFvariable *varParent = NULL;
//    CDFr::CDFreader   *readParent = NULL;

//    QVariant data;

//    switch(dataType)
//    {
//    //1 byte signed integer
//    case CDF_BYTE:

//    case CDF_CHAR:

//    case CDF_INT1:

//        break;

//    //1 byte unsigned integer
//    case CDF_UCHAR:

//    case CDF_UINT1:

//        break;

//    //2 byte signed integer
//    case CDF_INT2:

//        break;

//    //2 byte unsigned integer
//    case CDF_UINT2:

//        break;

//    //4 byte signed integer
//    case CDF_INT4:

//        break;

//    //4 byte unsigned integer
//    case CDF_UINT4:

//        break;

//    //8-byte signed integer
//    case CDF_INT8:

//    case CDF_TIME_TT2000:

//        break;

//    //4-byte floating point
//    case CDF_REAL4:

//    case CDF_FLOAT:

//        break;

//    //8-byte floating point
//    case CDF_REAL8:

//    case CDF_DOUBLE:

//    case CDF_EPOCH:

//        break;

//    //2 - 8-byte floating points
//    case CDF_EPOCH16:

//        break;

//    }


//    if(global)
//    {

//    }



//}

//void CDFr::CDFreader::gatherAttributes(void* parent, CDFid fileID, bool global)
//{
//    //Generate Attributes (Global Only)
//    //getting global attributes
//    {
//        CDFstatus status;

//        CDFr::CDFvariable *varParent = NULL;
//        CDFr::CDFreader   *readerParent = NULL;

//        if(global)
//        {
//            readerParent = (CDFr::CDFreader*)parent;
//        }
//        else
//        {
//            varParent = (CDFr::CDFvariable*)parent;
//        }

//        long numAttrs;
//        long entryN;
//        long attrN;
//        long attrScope;
//        long maxEntry;
//        long dataType;
//        long numElems;
//        char attrName[CDF_ATTR_NAME_LEN256+1];


//        status = CDFgetNumAttributes(fileID, &numAttrs);

//        //check for error status
//        if(this->setErrorStatus(status)) return;

//        //cycle through the attributes
//        for(long x = 0; x < numAttrs; x ++)
//        {
//            status = CDFattrInquire(fileID, x, attrName, &attrScope, &maxEntry);
//            if(this->setErrorStatus(status)) return;

//            //we only want global scope attributes
//            if(global)
//            {
//                if(attrScope != CDFr_GLOBAL_SCOPE) continue;
//            }
//            else
//            {
//                if(attrScope != CDFr_VARIABLE_SCOPE) continue;
//            }

//            //get the stuff
//            for(entryN=0; entryN <= maxEntry; entryN++)
//            {
//                status = CDFinquireAttrgEntry(fileID, attrN, entryN, &dataType, &numElems);

//                //No Such entry is an expected error
//                if(status != CDFr_NO_SUCH_ENTRY && status != CDFr_NO_SUCH_ATTR && this->setErrorStatus(status)) return;
//                else this->clearErrorStatus();

//                CDFr::CDFattribute *newAttr = new CDFr::CDFattribute(this);

//                //add relevent data
//                newAttr->setAttributeName(QString(attrName));
//                newAttr->setGlobalAtt();
//                newAttr->setType(dataType);

//                //process the actual attribute
//                this->gatherAttributes(this, fileID, true);

//                //add the attribute to the object
//                this->Attributes[QString(attrName)] = newAttr;

//            }

//        }
//    }
//}


