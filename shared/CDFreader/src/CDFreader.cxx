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
int64_t CDFr::CDFreader::getNumberVariables()
{

    return 0;
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
int64_t CDFr::CDFreader::getNumberGlobalAttributes()
{
    return 0;
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
QVariant CDFr::CDFreader::getCompressionType()
{

    return QVariant("getCompressionType UNIMPLEMENTED");
}

//==================================================================//
QVariant CDFr::CDFreader::getChecksum()
{

    return QVariant("getChecksum UNIMPLEMENTED");

}

//==================================================================//
QVariant CDFr::CDFreader::getFormat()
{

    return QVariant("getFormat UNIMPLEMENTED");

}

//==================================================================//
QVariant CDFr::CDFreader::getEncoding()
{

    return QVariant("getEncoding UNIMPLEMENTED");

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
    //start the error tracker
    this->errorTracker = new CDFr::CDFerrorHandler;

    //Record the File Name
    this->FileName = FileName;

    //Open the File
    long fileID;
    CDFstatus status = CDFopenCDF(this->FileName.toAscii().data(), &fileID);

    if(status != CDF_OK)
    {
        //foward the error status
        this->errorTracker->setError(status);
        std::cerr << "ERROR: " << this->getErrorString().toStdString() << std::endl;
        return;
    }


    //need to populate the header information for the object



}

//==================================================================//
CDFr::CDFreader::~CDFreader()
{
}
