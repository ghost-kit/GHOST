#ifndef ENLILEVOFILE_H
#define ENLILEVOFILE_H

#include <QMap>
#include <QString>
#include <QList>
#include <QStringList>
#include <QVector>
#include <QVariant>
#include <vtknetcdf/include/netcdf.h>
#include <vtknetcdf/cxx/netcdfcpp.h>



class enlilEvoFile
{
public:
    enlilEvoFile(const char *FileName);
    ~enlilEvoFile();

    void setFileName(const char *FileName);
    void setName(const char* name);

    QString getFileName();
    QString getName();
    qint64 getStepCount();
    QStringList getVarNames();
    QStringList getMeataDataNames();

    QVector<double> getVar(const char* name);
    QString getVarUnits(const char* name);
    QString getVarLongName(const char* name);

    QVariant getMetaData(const char* name);

protected:
    //basic info
    QString name;
    QString fileName;
    long stepCount;

    // data variables
    QMap<QString, QVector<double> > variables;
    QMap<QString, QString> units;
    QMap<QString, QString> longNames;

    // MetaData
    QMap<QString, QVariant> fileMetaData;

private:
    //file level
    void _loadVariable(QString name);
    void _loadMetaData(QString name);
    QStringList _getVaribleList();
    QStringList _getAttributeList();

    //var level
    void _loadAttFromVar(QString VarName, QString AttName);
    QStringList _getAttListForVar(QString varName);

    //file manipulations
    NcFile *file;

};

#endif // ENLILEVOFILE_H
