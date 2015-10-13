#ifndef ENLIL3DFILE_H
#define ENLIL3DFILE_H

#include <QMap>
#include <QString>
#include <QList>
#include <QStringList>
#include <QVector>
#include <QVariant>
#include <vtknetcdf/include/netcdf.h>
#include <vtknetcdf/cxx/netcdfcpp.h>
#include <QtAlgorithms>

class enlil3DFile
{
public:
    enlil3DFile(const char* FileName, double scaleFactor);
    ~enlil3DFile();

    void setFileName(const char *FileName);
    void setName(const char* name);
    void addUnitConversion(const char* baseUnits, const char* newUnits, double divisor);

    //File Level Routines
    QString getFileName();
    QString getName();
    int getDims(int xyz);
    int get3Dcount();
    double getMJD() const;
    double getScale_factor() const;
    void setScale_factor(double scale_factor);

    //Variable Routines
    QStringList getVarNames();
    QVector<double> getVar(const char* name);
    qint64 getNumberOfVars();

    //File Attribute Routines
    QStringList getFileAttributeNames();
    QVariant getFileAttribute(const char* name);
    int getNumberOfFileAttributes();

    //Variable Attribute Routines
    bool hasUnits(const char* name);
    bool hasUnits(QString name);
    QString getVarUnits(const char* name);
    QString getVarLongName(const char* name);

    //switch between raw and processed data
    void selectOutput(int version);




protected:
    //basic info
    QString name;
    QString fileName;
    double MJD;
    qint64 TIME;
    int dims[3];
    bool outputRaw;

    // data variables
    QMap<QString, QVector<double> > *varOutput;
    QMap<QString, QVector<double> > _variablesRaw;
    QMap<QString, QVector<double> > _variablesProcessed;

    // units variables
    QMap<QString, QString> *varUnitsOutput;
    QMap<QString, QString> _varUnitsRaw;
    QMap<QString, QString> _varUnitsProcessed;

    //long name variables
    QMap<QString, QString> *longNamesOutput;
    QMap<QString, QString> _longNamesRaw;
    QMap<QString, QString> _longNamesProcessed;

    // MetaData
    QMap<QString, QVariant> fileAttributeData;

private:
    //setup
    void _initializeFiles();

    //file level
    void _loadVariable(QString name);
    void _loadFileAttribute(QString name);
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

    /**
     * @brief runDataString
     * This string takes on the value for the reference date as per the version of the
     *      Enlil data file.
     * Version 2.6 and 2.7 -> refdate_mjd
     * Version 2.8 -> rundate_mjd
     */
    QString runDataString;

};

#endif // ENLIL3DFILE_H
