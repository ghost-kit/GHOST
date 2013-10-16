#ifndef CDFREADER_H
#define CDFREADER_H

#include "CDFattribute.h"
#include "CDFvariable.h"
#include "CDFerror.h"
#include "CDFglobalDefs.h"

#include <QString>
#include <QMap>
#include <QList>
#include <QVariant>

#include "cdf.h"



namespace CDFr
{

class CDFvariable;
class CDFattribute;
class CDFerrorHandler;


class CDFreader
{
public:
    //creator/destructor
    CDFreader(QString FileName);
    ~CDFreader();

    //data related infromation
    /* Variables */
    QString           getVarName(int index);
    QStringList       getVarNames();
    long              getNumberVariables();
    CDFvariable*      getVariable(int index);
    CDFvariable*      getVariable(QString name);

    /* Global Attributes */
    QString           getGlobalAttName(int index);
    QStringList       getGlobalAttNames();
    long              getNumberGlobalAttributes();
    CDFattribute*     getGlobalAttribute(int index);
    CDFattribute*     getGlobalAttribute(QString name);


    //file related information
    QString     getFileName();
    long        getCompressionType();
    long        getChecksum();
    long        getFormat();
    long        getEncoding();
    QVariant    getCdfVersion();
    bool        isRowMajor();

    //existence inquire
    bool        attributeExists(QString name);
    bool        variableExists(QString name);

    //error handling
    bool        hasError();         //if there is an error or not
    QString     getErrorString();   //returns the error string for the current error
    bool        setErrorStatus(CDFstatus status);

    QString     getCopyright() const;
    void        setCopyright(const QString &value);

    long getMajority() const;
    void setMajority(long value);

    static QString             getDateStringFromEPOCH(double epoch);
    static QString             getIso8601DateStringFromEPOCH(double epoch);

protected:

    //File Related information
    QString     FileName;
    QString     Copyright;
    long        CompressionType;
    QList<long> CompressionParms;
    long        CompressionPercentage;
    long        Format;
    long        Checksum;
    long        Encoding;
    long        cdfVersion;
    long        cdfRelease;
    long        cdfIncrement;
    long        majority;

    //data related information
    CDFr::variableList     Variables;
    CDFr::attributeList    Attributes;


private:
    CDFr::CDFerrorHandler*   errorTracker;
    void                     clearErrorStatus();

    CDFr::attributeList processAttributesList(CDFid fileID, long Scope, long VarNum=0, CDFvariable *variable = NULL);
    QList<QVariant>     *extractAttributeElements(CDFid fileID, long attrN, long entryNum, long scope);


};
}


#endif //CDFREADER_H
