/**
  * File: enlil3Dvar.h
  * Author: Joshua Murphy
  * Date: 26 Oct 2015
  *
  * Description:  This file contains the routins and classes necessary to handle Enlil 3D Data Variables and their associated attributes.
  *               These objects can only be used as childeren of an enlil3Dfile object.
  *
  */

//TODO: make sure we check for errors everywhere as to avoid inadvertant crashes.
//TODO: make sure all attributes are released after loaded (applies to NC_Att* returns)
//TODO: Make sure we can read a hypercube of data properly.
//TODO: Make sure we know how to handle variables that are a single data point.

#ifndef ENLILVAR_H
#define ENLILVAR_H


#include <QVector>
#include <QVariant>
#include <QString>
#include <QPair>
#include <QStack>

#include "enlilAtt.h"
#include "enlil3dfile.h"


typedef QPair<qint64,qint64> enlilExtent;
typedef QMap<QString, QPair <QString, double> > enlilConversion;
typedef QPair<QString, QPair<QString, double> > enlilConversionPair;

class enlil3DFile;
class enlilAtt;

/**
 * @brief The enlilVar class
 */
class enlil3DVar
{
    friend class enlil3DFile;
public:

    enlil3DVar(enlil3DFile* parent, QString Name, enlilConversion *conversionMap=NULL, enlilConversion *scaleFactor=NULL);
    ~enlil3DVar();

    QString longName() const;
    void setLongName(const QString &longName);

    bool cached() const;
    void setCached(bool cached);

    QString units();
    QString unitsBase();

    QVector<double> asDouble(int nblk=0);
    QVector<double> asDouble(QVector<qint64> extents, int nblk=0);
    QVector<double> asDouble(const int extents[6], int nblk=0);
    QVector<double> asDouble(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk=0);
    QVector<double> asDouble(QVector<enlilExtent> extents, int nblk=0);

    QVector<float> asFloat(int nblk=0);
    QVector<float> asFloat(QVector<qint64> extents, int nblk=0);
    QVector<float> asFloat(const int extents[6], int nblk=0);
    QVector<float> asFloat(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk=0);
    QVector<float> asFloat(QVector<enlilExtent> extents, int nblk=0);

    QVector<qint64> asInt64(int nblk=0);
    QVector<qint64> asInt64(QVector<qint64> extents, int nblk=0);
    QVector<qint64> asInt64(const int extents[6], int nblk=0);
    QVector<qint64> asInt64(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk=0);
    QVector<qint64> asInt64(QVector<enlilExtent> extents, int nblk=0);

    qint64 recordCount();

    enlilExtent getExtent(const char* name);
    QStringList getExtentNames(QString name);

    enlilConversion *getConversionFactor() const;
    void setConversionFactor(enlilConversion *value);
    void setConversionFactor(const char* oldUnits, const char* newUnits, double divisor);

    bool useConversions() const;
    void setUseConversions(bool useConversions);

private: //methods
    void _loadMetaData();

    QVector<QVariant> *_getData(enlilExtent N1, enlilExtent N2, enlilExtent N3, int nblk=0);
    QVector<QVariant> *_getVariantData(NcVar *var, int length, size_t counts[], long startLoc[]);
    enlilConversionPair _unitsPair();

private: //data

    enlil3DFile *__parent;

    qint64 __type;
    QStack<enlilConversionPair> __unitStack;
    QString __rawUnits;
    QString __convUnits;
    QString __scaledUnits;
    QString __varName;
    QString __varLongName;
    QMap<QString, enlilAtt*> __atts;
    QVector<QVariant> *__data;
    bool __cached;

    QMap<QString, qint64> __dims;
    QMap<QString, enlilExtent > __wholeExtents;
    qint64 __recordCount;

    enlilConversion *__conversionFactor;
    enlilConversion *__scaleFactor;
    double __conversionValue;
    bool __useConversions;

};

#endif // ENLILVAR_H
