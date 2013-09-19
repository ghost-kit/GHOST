#ifndef CDFREADER_H
#define CDFREADER_H

#include <QString>
#include <QMap>
#include <QList>
#include <QVariant>



namespace CDFr
{
    class CDFreader;
    class CDFattribute;
    class CDFvariable;
}

#include "CDFattribute.h"
#include "CDFvariable.h"

class CDFreader
{
public:
    //creator/destructor
    CDFreader();
    ~CDFreader();

    //data related infromation
    /* Variables */
    QString         getVarName(int index);
    QStringList     getVarNames();
    int64_t         getNumberVariables();
    CDFr::CDFvariable*    getVariable(int index);
    CDFr::CDFvariable*    getVariable(QString name);

    /* Global Attributes */
    QString         getGlobalAttName(int index);
    QStringList     getGlobalAttNames();
    int64_t         getNumberGlobalAttributes();
    CDFr::CDFattribute*   getGlobalAttribute(int index);
    CDFr::CDFattribute*   getGlobalAttribute(QString name);


    //file related information
    QString     getFileName();
    QVariant    getCompressionType();
    QVariant    getChecksum();
    QVariant    getFormat();
    QVariant    getEncoding();
    QVariant    getCdfVersion();
    bool        isRowMajor();

protected:

    //File Related information
    QString     FileName;
    QVariant    CompressionType;
    QVariant    Format;
    QVariant    Checksum;
    QVariant    Encoding;
    QVariant    cdfVersion;
    bool        majority;

    //data related information
    QMap<QString, CDFr::CDFvariable*>     Variables;
    QMap<QString, CDFr::CDFattribute*>    Attributes;

private:

};



#endif //CDFREADER_H
