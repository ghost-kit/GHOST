#ifndef CDFVARIABLE_H
#define CDFVARIABLE_H

#include "CDFreader.h"
#include "CDFattribute.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QVariant>


class CDFvariable
{
public:
    //constructor/destructor
    CDFvariable(CDFr::CDFreader *parent);
    ~CDFvariable();

    //file related information
    CDFr::CDFreader* getParent();

    //Variable related information
    QString             getVarName();
    int64_t             getVarType();
    int64_t             getNumberDims();
    int64_t             getDim(int dim);
    int64_t*            getAllDims();
    int64_t             getNumberEntries();
    int64_t             getNumberAtts();
    int64_t             getMonotonicState();
    bool                isZvar();

    //actual data
    QVariant            getFillValue();
    QVariant            getMinimum();
    QVariant            getMaximum();
    QVariant            getDataItem(int64_t index);

    //Attributes
    CDFr::CDFattribute  getAttribute(int64_t index);
    CDFr::CDFattribute  getAttirbute(QString name);

    //index conversions
    inline int64_t      fromXYZ(int64_t x, int64_t y, int64_t z);
    inline int64_t      fromXY(int64_t x, int64_t y);

protected:
    //file related information
    CDFr::CDFreader *parent;

    //Variable Related information
    QList<int64_t>  Dims;
    int64_t         VarType;
    QVariant        fillValue;
    QVariant        minimum;
    QVariant        maximum;
    bool            zVar;

    //variance status (does the data change?)
    bool            recordVaries;               //Data Record Variance
    QList<bool>     dimVaries;                  //Dimensions Variance

    //data related info
    QMap<int64_t, QVariant> Data;               //data map[index_from_file] -> data point
    QMap<QString, CDFr::CDFattribute*> Attributes;    //Variable attributes maped by name

private:


};


#endif //CDFVARIABLE_H
