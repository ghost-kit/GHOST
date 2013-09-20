#include "CDFreader.h"
#include "CDFattribute.h"
#include "CDFvariable.h"


using namespace CDFr;


//==================================================================//
QString CDFr::CDFreader::getVarName(int index)
{

    return QString('NULL');
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
    CDFvariable *variable = new CDFvariable(this);

    return variable;
}

//==================================================================//
CDFr::CDFvariable *CDFr::CDFreader::getVariable(QString name)
{
}

//==================================================================//
QString CDFr::CDFreader::getGlobalAttName(int index)
{
}

//==================================================================//
QStringList CDFr::CDFreader::getGlobalAttNames()
{
}

//==================================================================//
int64_t CDFr::CDFreader::getNumberGlobalAttributes()
{
}

//==================================================================//
CDFr::CDFattribute* CDFr::CDFreader::getGlobalAttribute(int index)
{
}

//==================================================================//
CDFr::CDFattribute* CDFr::CDFreader::getGlobalAttribute(QString name)
{
}

//==================================================================//
QString CDFr::CDFreader::getFileName()
{
}

//==================================================================//
QVariant CDFr::CDFreader::getCompressionType()
{
}

//==================================================================//
QVariant CDFr::CDFreader::getChecksum()
{
}

//==================================================================//
QVariant CDFr::CDFreader::getFormat()
{
}

//==================================================================//
QVariant CDFr::CDFreader::getEncoding()
{
}

//==================================================================//
QVariant CDFr::CDFreader::getCdfVersion()
{
}

//==================================================================//
bool CDFr::CDFreader::isRowMajor()
{
}

//==================================================================//
void CDFr::CDFreader::setBadDataHandler(CDFbadDataHandler *handler)
{
}

