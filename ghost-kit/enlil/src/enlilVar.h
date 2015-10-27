#ifndef ENLILVAR_H
#define ENLILVAR_H

#include <QVector>
#include <QVariant>
#include <QString>
#include <QPair>

#include "enlil3dfile.h"

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
class enlilVar
{
public:
    enlilVar(enlil3DFile *__parent, qint64 type, QString Name);
    ~enlilVar();

    QString varLongName() const;
    void setVarLongName(const QString &varLongName);

    bool cached() const;
    void setCached(bool cached);

    QVector<QVariant>* getData();
    QVector<QVariant> *getData(QVector<qint64> extents);
    QVector<QVariant> *getData(const int extents[6]);
    QVector<QVariant> *getData(QPair<qint64,qint64> X, QPair<qint64,qint64> Y, QPair<qint64,qint64> Z);

    qint64 recordCount();

    QPair<qint64,qint64> getXextent();
    void setXextent(qint64 lower, qint64 upper);

    QPair<qint64,qint64> getYextent();
    void setYextent(qint64 lower, qint64 upper);

    QPair<qint64,qint64> getZextent();
    void setZextent(qint64 lower, qint64 upper);


private:

    enlil3DFile* __parent;

    qint64 __type;
    QString __units;
    QString __varName;
    QString __varLongName;
    QVector<enlilAtts> *__atts;
    QVector<QVariant> *__data;
    bool __cached;

    QMap<QString, qint64> __dims;
    QMap<QString, QPair<qint64,qint64> > __wholeExtents;
    qint64 __recordCount;



};

#endif // ENLILVAR_H
