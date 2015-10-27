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
//TODO: load attributes using parent file object
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

#include "enlil3dfile.h"

typedef QPair<qint64,qint64> enlilExtent;


/**
 * @brief The enlilAtts class
 */
class enlilAtts
{
public:
    enlilAtts(int type, QString name, QVariant value);
    ~enlilAtts();

    int getType() const;

    QVariant getValue() const;

    QString getAttName() const;


private:

    int __type;
    QString __attName;
    QVariant __value;

};

/**
 * @brief The enlilVar class
 */
class enlil3DVar
{
public:
    enlil3DVar(enlil3DFile *__parent, QString Name);
    ~enlil3DVar();

    QString varLongName() const;
    void setVarLongName(const QString &varLongName);

    bool cached() const;
    void setCached(bool cached);

    QVector<QVariant>* getData();
    QVector<QVariant> *getData(QVector<qint64> extents);
    QVector<QVariant> *getData(const int extents[6]);
    QVector<QVariant> *getData(enlilExtent X, enlilExtent Y, enlilExtent Z);
    QVector<QVariant> *getData(QVector<enlilExtent>);

    qint64 recordCount();

    QPair<qint64,qint64> getXextent();
    void setXextent(qint64 lower, qint64 upper);

    QPair<qint64,qint64> getYextent();
    void setYextent(qint64 lower, qint64 upper);

    QPair<qint64,qint64> getZextent();
    void setZextent(qint64 lower, qint64 upper);

private: //methods
    void _loadData(QVector<enlilExtent> subExtents);
    void _processMetaData();


private: //data

    enlil3DFile* __parent;

    qint64 __type;
    QString __units;
    QString __varName;
    QString __varLongName;
    QVector<enlilAtts> *__atts;
    QVector<QVariant> *__data;
    bool __cached;

    QMap<QString, qint64> __dims;
    QMap<QString, enlilExtent > __wholeExtents;
    qint64 __recordCount;



};

#endif // ENLILVAR_H
