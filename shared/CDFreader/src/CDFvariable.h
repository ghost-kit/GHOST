#ifndef CDFVARIABLE_H
#define CDFVARIABLE_H

#include "CDFreader.h"
#include "CDFattribute.h"
#include "CDFBadDataHandler.h"
#include "CDFglobalDefs.h"
#include "CDFerror.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QVariant>



namespace CDFr
{
class CDFattribute;
class CDFbadDataHandler;
class CDFreader;
class CDFerrorHandler;

class CDFvariable
{
public:
    //constructor/destructor
    CDFvariable(CDFr::CDFreader *parent);
    ~CDFvariable();

    //file related information
    CDFreader* getParent();

    //Variable related information
    QString             getVarName();
    void                setVarName(QString name);

    long                getVarType();
    void                setVarType(long type);

    int64_t             getNumberDims();

    int64_t             getDim(int dim);
    void                setDim(long index, int dimValue);

    int64_t*            getAllDims();
    void                setAllDims(long dims[], int count);

    long             getNumberEntries();
    void                setNumberEntries(long numEntries);

    long                 getNumberAtts();
    void                addAttribute(CDFattribute *attribute);

    int64_t             getMonotonicState();
    void                setMonotonicState(long state);


    //actual data
    QVariant            getFillValue();
    void                setFillValue(QVariant fill);

    QVariant            getMinimum();
    void                setMinimum(QVariant minimum);

    QVariant            getMaximum();
    void                setMaximum(QVariant maximum);

    QVariant            getDataItem(int64_t index);

    //Attributes
    CDFattribute *getAttribute(int64_t index);
    CDFattribute *getAttirbute(QString name);

    bool          attributeExists(QString name);


    //index conversions
    inline int64_t      fromXYZ(int64_t x, int64_t y, int64_t z);
    inline int64_t      fromXY(int64_t x, int64_t y);

    long getDimVaries(long index) const;
    void setDimVaries(const long value[], long count);

    bool getRecordVaries() const;
    void setRecordVaries(bool value);

    void setAttributeList(CDFr::attributeList Attributes);

protected:
    //file related information
    CDFreader *parent;

    //Variable Related information
    QString         Name;
    QMap<long, long>  Dims;
    int64_t         VarType;
    int64_t         numEntries;
    QVariant        fillValue;
    QVariant        minimum;
    QVariant        maximum;
    long              monotonicState;


    //variance status (does the data change?)
    long            recordVaries;               //Data Record Variance
    QList<bool>     dimVaries;                  //Dimensions Variance

    //data related info
    QMap<int64_t, QVariant>  Data;               //data map[index_from_file] -> data point
    CDFr::attributeList      Attributes;         //Variable attributes maped by name


private:
    bool errorState;

};
}

#endif //CDFVARIABLE_H
