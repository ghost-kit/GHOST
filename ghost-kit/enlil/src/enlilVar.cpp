#include "enlilVar.h"


//ENLIL VARIABLES

/**
 * @brief enlilVar::enlilVar
 * @param FileName
 * @param type
 * @param Name
 */
enlilVar::enlilVar(enlil3DFile *parent, qint64 type, QString Name)
{
    this->__parent = parent;
    this->__type = type;
    this->__varName = Name;

    this->__cached = false;
    this->__data = NULL;
    this->__atts = NULL;
}

/**
 * @brief enlilVar::~enlilVar
 */
enlilVar::~enlilVar()
{

}

/**
 * @brief enlilVar::varLongName
 * @return
 */
QString enlilVar::varLongName() const
{
    return __varLongName;
}

/**
 * @brief enlilVar::setVarLongName
 * @param varLongName
 */
void enlilVar::setVarLongName(const QString &varLongName)
{
    __varLongName = varLongName;
}

/**
 * @brief enlilVar::cached
 * @return
 */
bool enlilVar::cached() const
{
    return __cached;
}

/**
 * @brief enlilVar::setCached
 * @param cache
 */
void enlilVar::setCached(bool cache)
{
    __cached = cache;
}

/**
 * @brief enlilVar::getData
 * @return
 */
QVector<QVariant> *enlilVar::getData()
{

}

QVector<QVariant> *enlilVar::getData(QVector<qint64> extents)
{

}

/**
 * @brief enlilVar::getData
 * @param extents
 * @return
 */
QVector<QVariant> *enlilVar::getData(const int extents[])
{

}

QVector<QVariant> *enlilVar::getData(QPair<qint64, qint64> X, QPair<qint64, qint64> Y, QPair<qint64, qint64> Z)
{

}

/**
 * @brief enlilVar::recordCount
 * @return
 */
qint64 enlilVar::recordCount()
{

}

/**
 * @brief enlilVar::getXextent
 * @return
 */
QPair<qint64, qint64> enlilVar::getXextent()
{

}

/**
 * @brief enlilVar::setXextent
 * @param lower
 * @param upper
 */
void enlilVar::setXextent(qint64 lower, qint64 upper)
{

}

/**
 * @brief enlilVar::setYextent
 * @param lower
 * @param upper
 */
void enlilVar::setYextent(qint64 lower, qint64 upper)
{

}

/**
 * @brief enlilVar::getZextent
 * @return
 */
QPair<qint64, qint64> enlilVar::getZextent()
{

}

/**
 * @brief enlilVar::setZextent
 * @param lower
 * @param upper
 */
void enlilVar::setZextent(qint64 lower, qint64 upper)
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
