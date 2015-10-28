/**
  * File: enlil3Dvar.h
  * Author: Joshua Murphy
  * Date: 26 Oct 2015
  *
  * Description:  This file contains the routins and classes necessary to handle Enlil 3D Data Variables and their associated attributes.
  *               These objects can only be used as childeren of an enlil3Dfile object.
  *
  */

//TODO: load variable data using parent file object
//TODO: configure transforms for the variable
//TODO: make sure we have units and long names
//TODO: make sure transforms change the units tags
//TODO: make sure we can change the names of different blocks within nblk for easier export. (i.e. name them background and full, or something)
//TODO: make sure we have number of records
//TODO: make sure we can switch between records easily.
//TODO: make sure we can have the STATE of the variable set (which record is being pointed to)
//TODO: make sure we check for errors everywhere as to avoid inadvertant crashes.
//TODO: make sure all attributes are released after loaded (applies to NC_Att* returns)
//TODO: Make sure we can read a hypercube of data properly.
//TODO: Make sure we know how to handle variables that are a single data point.
//TODO: Make sure we can get data "as_double", "as_float", "as_int", "as_string", etc.

#ifndef ENLILVAR_H
#define ENLILVAR_H


#include <QVector>
#include <QVariant>
#include <QString>
#include <QPair>

#include "enlilAtt.h"
#include "enlil3dfile.h"


typedef QPair<qint64,qint64> enlilExtent;

class enlil3DFile;
class enlilAtt;

/**
 * @brief The enlilVar class
 */
class enlil3DVar
{
    friend class enlil3DFile;
public:

    enlil3DVar(enlil3DFile* parent, QString Name);
    ~enlil3DVar();

    QString LongName() const;
    void setVarLongName(const QString &LongName);

    bool cached() const;
    void setCached(bool cached);

    QString Units();


    QVector<double> asDouble(int nblk=0);
    QVector<double> asDouble(QVector<qint64> extents, int nblk=0);
    QVector<double> asDouble(const int extents[6], int nblk=0);
    QVector<double> asDouble(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk=0);
    QVector<double> asDouble(QVector<enlilExtent>, int nblk=0);

    QVector<float> asFloat(int nblk=0);
    QVector<float> asFloat(QVector<qint64> extents, int nblk=0);
    QVector<float> asFloat(const int extents[6], int nblk=0);
    QVector<float> asFloat(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk=0);
    QVector<float> asFloat(QVector<enlilExtent>, int nblk=0);

    QVector<qint64> asInt64(int nblk=0);
    QVector<qint64> asInt64(QVector<qint64> extents, int nblk=0);
    QVector<qint64> asInt64(const int extents[6], int nblk=0);
    QVector<qint64> asInt64(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk=0);
    QVector<qint64> asInt64(QVector<enlilExtent>, int nblk=0);

    QVector<QVariant> *getData(int nblk=0);
    QVector<QVariant> *getData(QVector<qint64> extents, int nblk=0);
    QVector<QVariant> *getData(const int extents[6], int nblk=0);
    QVector<QVariant> *getData(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk=0);
    QVector<QVariant> *getData(QVector<enlilExtent>, int nblk=0);

    qint64 recordCount();

    QPair<qint64,qint64> getXextent();
    void setXextent(qint64 lower, qint64 upper);

    QPair<qint64,qint64> getYextent();
    void setYextent(qint64 lower, qint64 upper);

    QPair<qint64,qint64> getZextent();
    void setZextent(qint64 lower, qint64 upper);

private: //methods
    void _loadData(QVector<enlilExtent> subExtents, int nblk);
    void _loadMetaData();


private: //data

    enlil3DFile *__parent;

    qint64 __type;
    QString __units;
    QString __varName;
    QString __varLongName;
    QMap<QString, enlilAtt*> __atts;
    QVector<QVariant> *__data;
    bool __cached;

    QMap<QString, qint64> __dims;
    QMap<QString, enlilExtent > __wholeExtents;
    qint64 __recordCount;



    QVector<QVariant> *getVariantData(NcVar *var, int length, size_t counts[], long startLoc[]);
};

#endif // ENLILVAR_H
