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
#include <QtAlgorithms>


class enlilEvoFile
{
public:
    enlilEvoFile(const char *FileName, double scaleFactor);
    ~enlilEvoFile();

    void setFileName(const char *FileName);
    void setName(const char* name);
    void addUnitConversion(const char* baseUnits, const char* newUnits, double divisor);

    QString getFileName();
    QString getName();
    qint64 getStepCount();
    QStringList getVarNames();
    QStringList getMeataDataNames();

    bool hasUnits(const char* name);
    bool hasUnits(QString name);

    QVector<double> getVar(const char* name);
    QString getVarUnits(const char* name);
    QString getVarLongName(const char* name);

    QVariant getMetaData(const char* name);

    //switch between raw and processed data
    void selectOutput(int version);


protected:
    //basic info
    QString name;
    QString fileName;
    long stepCount;
    bool outputRaw;

    // data variables
    QMap<QString, QVector<double> > varOutput;
    QMap<QString, QVector<double> > _variablesRaw;
    QMap<QString, QVector<double> > _variablesProcessed;

    // units variables
    QMap<QString, QString> varUnitsOutput;
    QMap<QString, QString> _varUnitsRaw;
    QMap<QString, QString> _varUnitsProcessed;

    //long name variables
    QMap<QString, QString> longNamesOutput;
    QMap<QString, QString> _longNamesRaw;
    QMap<QString, QString> _longNamesProcessed;

    // MetaData
    QMap<QString, QVariant> fileMetaData;

private:
    //setup
    void _initializeFiles();

    //file level
    void _loadVariable(QString name);
    void _loadMetaData(QString name);
    QStringList _getVaribleList();
    QStringList _getAttributeList();

    //var level
    void _loadAttFromVar(QString VarName, QString AttName);
    QStringList _getAttListForVar(QString varName);

    //data processing
    void _processLocation();
    void _processSphericalVectors();
    void _processScalars();
    void _processTime();

    //data conversions
    //convMap holds conversions: form convMap[rawUnits][newUnits]=divisor
    QMap<QString, QPair<QString, double> > _convMap;
    void _addConversion(QString baseUnits, QString newUnits, double divisor);
    QPair<QString, double> _getConvDivForVar(QString var);


    //coordinate transforms (the basic ones)
    //these vectors MUST be 3 tuples
    //it is the responsibility of the calling function to
    //   free the memory associated with these conversions
    double *_gridSphere2Cart(const double rtp[]);
    double *_sphere2Cart(const double rtp[], const double rtpOrigin[]);

    // how much to scale the grid
    double __scale_factor;

    //file manipulations
    NcFile *file;

    double getMax(QVector<double> vector);

};

#endif // ENLILEVOFILE_H
