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

    return NULL;
}

//==================================================================//
CDFr::CDFvariable *CDFr::CDFreader::getVariable(QString name)
{

    return NULL;
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
void CDFr::CDFreader::setBadDataHandler(CDFbadDataHandler *handler)
{
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
    long fileID;

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

            //TODO: populate attributes



            //add the variable to the list
            this->Variables[QString(varName)] = newVar;
        }
    }


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
void CDFr::CDFreader::processAttribute(const long dataType, const long numElemets, const long attN, const long entryN, QVariant &Data)
{
}
