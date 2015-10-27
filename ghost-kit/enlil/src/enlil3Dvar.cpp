/**
  * File: enlil3Dvar.cpp
  * Author: Joshua Murphy
  * Date: 26 Oct 2015
  *
  * Description:  This file contains the routins and classes necessary to handle Enlil 3D Data Variables and their associated attributes.
  *               These objects can only be used as childeren of an enlil3Dfile object.
  *
  */

#include "enlil3Dvar.h"


//ENLIL VARIABLES

/**
 * @brief enlilVar::enlilVar
 * @param FileName
 * @param type
 * @param Name
 */
enlil3DVar::enlil3DVar(enlil3DFile *parent, QString Name)
{
    //record basic information (parent file object, variable name)
    this->__parent = parent;
    this->__varName = Name;

    //mark default conditions
    this->__cached = false;
    this->__data = NULL;
    this->__atts = NULL;

    //load meta-data into variable
    this->_processMetaData();
}

/**
 * @brief enlilVar::~enlilVar
 */
enlil3DVar::~enlil3DVar()
{

}

/**
 * @brief enlilVar::varLongName
 * @return
 */
QString enlil3DVar::varLongName() const
{
    return __varLongName;
}

/**
 * @brief enlilVar::setVarLongName
 * @param varLongName
 */
void enlil3DVar::setVarLongName(const QString &varLongName)
{
    __varLongName = varLongName;
}

/**
 * @brief enlilVar::cached
 * @return
 */
bool enlil3DVar::cached() const
{
    return __cached;
}

/**
 * @brief enlilVar::setCached
 * @param cache
 */
void enlil3DVar::setCached(bool cache)
{
    __cached = cache;
}

/**
 * @brief enlilVar::getData
 * @return
 */
QVector<QVariant> *enlil3DVar::getData()
{

}

/**
 * @brief enlilVar::getData
 * @param extents
 * @return
 */
QVector<QVariant> *enlil3DVar::getData(QVector<qint64> extents)
{

}

/**
 * @brief enlilVar::getData
 * @param extents
 * @return
 */
QVector<QVariant> *enlil3DVar::getData(const int extents[])
{

}

/**
 * @brief enlilVar::getData
 * @return
 */
QVector<QVariant> *enlil3DVar::getData(QVector<enlilExtent>)
{

}

/**
 * @brief enlilVar::getData
 * @param X
 * @param Y
 * @param Z
 * @return
 */
QVector<QVariant> *enlil3DVar::getData(enlilExtent X, enlilExtent Y, enlilExtent Z)
{

}

/**
 * @brief enlilVar::recordCount
 * @return
 */
qint64 enlil3DVar::recordCount()
{

}

/**
 * @brief enlilVar::getXextent
 * @return
 */
QPair<qint64, qint64> enlil3DVar::getXextent()
{

}

/**
 * @brief enlilVar::setXextent
 * @param lower
 * @param upper
 */
void enlil3DVar::setXextent(qint64 lower, qint64 upper)
{

}

/**
 * @brief enlilVar::setYextent
 * @param lower
 * @param upper
 */
void enlil3DVar::setYextent(qint64 lower, qint64 upper)
{

}

/**
 * @brief enlilVar::getZextent
 * @return
 */
QPair<qint64, qint64> enlil3DVar::getZextent()
{

}

/**
 * @brief enlilVar::setZextent
 * @param lower
 * @param upper
 */
void enlil3DVar::setZextent(qint64 lower, qint64 upper)
{

}

/**
 * @brief enlilVar::_loadData
 * @param subExtents
 */
void enlil3DVar::_loadData(QVector<enlilExtent> subExtents)
{

}

/**
 * @brief enlilVar::_processMetaData
 */
void enlil3DVar::_processMetaData()
{

}


//ENLIL ATTRIBUTES

/**
 * @brief enlilAtts::enlilAtts
 * @param type
 * @param name
 * @param value
 */
enlilAtts::enlilAtts(int type, QString name, QVariant value)
{
    this->__type = type;
    this->__attName = name;
    this->__value = value;
}

/**
 * @brief enlilAtts::~enlilAtts
 */
enlilAtts::~enlilAtts()
{

}

/**
 * @brief enlilAtts::getValue
 * @return
 */
QVariant enlilAtts::getValue() const
{
    return __value;
}

/**
 * @brief enlilAtts::getAttName
 * @return
 */
QString enlilAtts::getAttName() const
{
    return __attName;
}

/**
 * @brief enlilAtts::getType
 * @return
 */
int enlilAtts::getType() const
{
    return __type;
}
