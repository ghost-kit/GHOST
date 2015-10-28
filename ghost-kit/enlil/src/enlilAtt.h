#ifndef ENLILATT_H
#define ENLILATT_H

#include <QString>
#include <QVariant>
#include <QVector>

/**
 * @brief The enlilAtts class
 */
class enlilAtt
{
public:
    enlilAtt(int type, QString name, QVariant value);
    ~enlilAtt();

    int getType() const;

    QVariant getValue() const;

    QString getAttName() const;


private:

    int __type;
    QString __attName;
    QVariant __value;

};


#endif // ENLILATT_H
