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

#include "enlil3Dvar.h"


#define ENLIL_GRIDSPACING_CT 0
#define ENLIL_GRIDSPACING_SP 1

class enlil3DVar;

class enlil3DFile
{
    friend class enlil3DVar;
public:
    // Class Level Functions
    enlil3DFile(const char* FileName, double scaleFactor);
    ~enlil3DFile();

    //Reset Routines
    void cleanVar(const char* _name);
    void setExtents(int extents[]);
    void setExtents(int X1, int X2, int Y1, int Y2, int Z1, int Z2);
    QVector<int> getExtents();

    //Easy Name
    void setName(const char* _name);

    //File Level Routines
    void setFileName(const char *FileName);
    QString getFileName();
    QString getName();
    double getMJD() const;
    double getScale_factor() const;
    void setScale_factor(double scale_factor);

    //Grid Spacing
    QVector<QVector<double> > getGridSpacing();
    int getDims(const char* dim);
    int get3Dcount();
    QString getGridUnits();
    void setGridSpacingType(int type);

    //Variable Routines
    QStringList getVarNames();
    QVector<double> getVar(const char* _name);
    QVector<QVector<double> > getVar(const char* X, const char* Y, const char* Z);
    qint64 getNumberOfVars();

    //File Attribute Routines
    QStringList getFileAttributeNames();
    enlilAtt *getFileAttribute(const char* _name);
    int getNumberOfFileAttributes();

    //Variable Attribute Routines
    bool hasUnits(const char* _name);
    bool hasUnits(QString _name);
    QString getVarUnits(const char* _name);
    QString getVarLongName(const char* _name);

    //switch between raw and processed data
    void selectOutput(int version);
    void addUnitConversion(const char* baseUnits, const char* newUnits, double divisor);


protected:  //DATA
    //basic info
    QString _name;
    QString _fileName;
    NcFile *_file;

    //Data Keeping
    double _MJD;
    double _TIME;
    QMap<QString, qint64> _dims;
    double _enlil_version;
    int _extents[6];

    bool _retainRaw;      //Flag to determine if we will retain the raw data in memory.
    bool _convertData;    //Flag to determine if we convert the raw data
    bool _useExtents;     //Use the set extents

    //grid Data
    QVector<QVector<double> > *_gridOutput;      //Current Output
    QVector<QVector<double> > _gridPositionsCT;  //Cartesian
    QVector<QVector<double> > _gridPositionsSP;  //Spherical
    QString _gridUnits;                 //maintain the units for grid scaling
    double _gridScaleFactor;            // how much to scale the grid

    //data variables
    QMap<QString, enlil3DVar* > _varOutput;

    // MetaData Variables
    QMap<QString, enlilAtt* > _fileAttributeData;

    // Converstion Variables
    QMap<QString, QPair<QString, double> > _convMap;

    /**
     * @brief runDataString
     * This string takes on the value for the reference date as per the version of the
     *      Enlil data file.
     * Version 2.6 and 2.7 -> refdate_mjd
     * Version 2.8 -> rundate_mjd
     *
     * FIXME: This should be a dictionary defining the different versions, and should be static.
     */
    QString _runDataString;

protected:  //METHODS
    //setup
    void __initializeFiles();

    //file level
    void __loadVariable(QString _name);
    void __loadVariableExtents(QString _name);
    void __loadFileAttribute(QString _name);
    QStringList __getVaribleList();
    QStringList __getAttributeList();

    //var level
    void __loadAttFromVar(QString VarName, QString AttName);
    QStringList __getAttListForVar(QString varName);

    //data processing
    void __cleanAll();
    void __processGridLocations();
    void __processSphericalVectors();
    void __processScalars();
    void __processTime();
    void __setUseExtents(int extents[]);
    QVector<int> __getExtentDimensions(int extent[6]);



    //data conversions
    //convMap holds conversions: form convMap[rawUnits][newUnits]=divisor
    void __addConversion(QString baseUnits, QString newUnits, double divisor);
    QPair<QString, double> __getConvDivForVar(QString var);

    //coordinate transforms (the basic ones)
    //these vectors MUST be 3 tuples
    //it is the responsibility of the calling function to
    //   free the memory associated with these conversions
    QVector<double> __gridSphere2Cart(const QVector<double> rtp);
    double *__sphere2Cart(const double rtp[], const double rtpOrigin[]);

    //file manipulations
    double __getMax(QVector<double> vector);

    void __ResetFile();
};

#endif // ENLIL3DFILE_H
