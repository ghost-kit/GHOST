#ifndef CDFREADER_H
#define CDFREADER_H

#include "CDFattribute.h"
#include "CDFvariable.h"
#include "CDFBadDataHandler.h"
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
class CDFbadDataHandler;
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
    long getNumberVariables();
    CDFvariable*      getVariable(int index);
    CDFvariable*      getVariable(QString name);

    /* Global Attributes */
    QString           getGlobalAttName(int index);
    QStringList       getGlobalAttNames();
    long getNumberGlobalAttributes();
    CDFattribute*     getGlobalAttribute(int index);
    CDFattribute*     getGlobalAttribute(QString name);


    //file related information
    QString     getFileName();
    long    getCompressionType();
    long    getChecksum();
    long getFormat();
    long getEncoding();
    QVariant    getCdfVersion();
    bool        isRowMajor();

    //existence inquire
    bool attributeExists(QString name);
    bool variableExists(QString name);

    //handlers
    void        setBadDataHandler(CDFr::CDFbadDataHandler *handler);

    //error handling
    bool    hasError();         //if there is an error or not
    QString getErrorString();   //returns the error string for the current error
    bool setErrorStatus(CDFstatus status);

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

    //Handlers
    CDFr::CDFbadDataHandler*        BadDataHandler;


private:
    CDFr::CDFerrorHandler*   errorTracker;
    void clearErrorStatus();

    CDFr::attributeList processAttributesList(CDFid fileID, long Scope, long VarNum=0, CDFvariable *variable = NULL);
    QList<QVariant> *extractAttributeElements(CDFid fileID, long attrN, long entryNum, long scope);


};
}


#endif //CDFREADER_H
