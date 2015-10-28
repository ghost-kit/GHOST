#include "enlilAtt.h"
#include <QString>
#include <QVariant>
//ENLIL ATTRIBUTES

/**
 * @brief enlilAtts::enlilAtts
 * @param type
 * @param name
 * @param value
 */
enlilAtt::enlilAtt(int type, QString name, QVariant value)
{
    this->__type = type;
    this->__attName = name;
    this->__value = value;
}

/**
 * @brief enlilAtts::~enlilAtts
 */
enlilAtt::~enlilAtt()
{

}

/**
 * @brief enlilAtts::getValue
 * @return
 */
QVariant enlilAtt::getValue() const
{
    return __value;
}

/**
 * @brief enlilAtts::getAttName
 * @return
 */
QString enlilAtt::getAttName() const
{
    return __attName;
}

/**
 * @brief enlilAtts::getType
 * @return
 */
int enlilAtt::getType() const
{
    return __type;
}
