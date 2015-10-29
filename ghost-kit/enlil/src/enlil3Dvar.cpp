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
#include <QString>
//ENLIL VARIABLES

/**
 * @brief enlilVar::enlilVar
 * @param FileName
 * @param type
 * @param Name
 */
enlil3DVar::enlil3DVar(enlil3DFile* parent, QString Name)
{
    //record basic information (parent file object, variable name)
    this->__parent = parent;
    this->__varName = Name;

    //mark default conditions
    this->__cached = false;
    this->__data = NULL;

    //load meta-data into variable
    this->_loadMetaData();
}

/**
 * @brief enlilVar::~enlilVar
 */
enlil3DVar::~enlil3DVar()
{

    //delete stored data
    if(this->__data)
    {
        delete this->__data;
        this->__data = NULL;
    }

    //delete attributes
    QStringList attNames = this->__atts.keys();
    for(int loop=0; loop < attNames.count(); loop++)
    {
        delete this->__atts[attNames[loop]];
    }
    this->__atts.clear();

}

/**
 * @brief enlilVar::varLongName
 * @return
 */
QString enlil3DVar::LongName() const
{
    return __varLongName;
}

/**
 * @brief enlilVar::setVarLongName
 * @param varLongName
 */
void enlil3DVar::setLongName(const QString &varLongName)
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
 * @brief enlil3DVar::Units
 * @return
 */
QString enlil3DVar::Units()
{
    return this->__units;
}

/**
 * @brief enlil3DVar::asDouble
 * @param nblk
 * @return
 */
QVector<double> enlil3DVar::asDouble(int nblk)
{

    enlilExtent X = this->__wholeExtents["n1"];
    enlilExtent Y = this->__wholeExtents["n2"];
    enlilExtent Z = this->__wholeExtents["n3"];

    return this->asDouble(X,Y,Z, nblk);

}

/**
 * @brief enlil3DVar::asDouble
 * @param extents
 * @param nblk
 * @return
 */
QVector<double> enlil3DVar::asDouble(QVector<qint64> extents, int nblk)
{
enlilExtent X,Y,Z;
X.first = extents[0];
X.second = extents[1];
Y.first = extents[2];
Y.second = extents[3];
Z.first = extents[4];
Z.second = extents[5];

return this->asDouble(X,Y,Z,nblk);
}

/**
 * @brief enlil3DVar::asDouble
 * @param extents
 * @param nblk
 * @return
 */
QVector<double> enlil3DVar::asDouble(const int extents[], int nblk)
{
enlilExtent X, Y, Z;
X.first = extents[0];
X.second = extents[1];
Y.first = extents[2];
Y.second = extents[3];
Z.first = extents[4];
Z.second = extents[5];

return this->asDouble(X,Y,Z,nblk);

}

/**
 * @brief enlil3DVar::asDouble
 * @param X
 * @param Y
 * @param Z
 * @param nblk
 * @return
 */
QVector<double> enlil3DVar::asDouble(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk)
{
    QVector<QVariant> *raw = this->_getData(X,Y,Z,nblk);
    QVector<double> asDouble;

    if(!raw)
    {
        std::cerr << "ERROR: Failed to get Variable Data for " << qPrintable(this->__varLongName) << std::endl;
        exit(1);
    }

    int count = raw->count();

    for(int x = 0; x < count; x++)
    {
        bool status;
        double value = raw[0][x].toDouble(&status);
        if(!status)
        {
            std::cerr << "FAILURE TO CONVERT TO DOUBLE." << std::endl;
        }
        else
        {
            asDouble.push_back(value);
        }
    }

    delete raw;
    return asDouble;
}

/**
 * @brief enlil3DVar::asDouble
 * @param nblk
 * @return
 */
QVector<double> enlil3DVar::asDouble(QVector<enlilExtent> extents, int nblk)
{
    return this->asDouble(extents[0],extents[1],extents[2],nblk);

}

/**
 * @brief enlil3DVar::asFloat
 * @param nblk
 * @return
 */
QVector<float> enlil3DVar::asFloat(int nblk)
{
    enlilExtent X = this->__wholeExtents["n1"];
    enlilExtent Y = this->__wholeExtents["n2"];
    enlilExtent Z = this->__wholeExtents["n3"];

    return this->asFloat(X,Y,Z, nblk);
}

/**
 * @brief enlil3DVar::asFloat
 * @param extents
 * @param nblk
 * @return
 */
QVector<float> enlil3DVar::asFloat(QVector<qint64> extents, int nblk)
{
    enlilExtent X,Y,Z;
    X.first = extents[0];
    X.second = extents[1];
    Y.first = extents[2];
    Y.second = extents[3];
    Z.first = extents[4];
    Z.second = extents[5];

    return this->asFloat(X,Y,Z,nblk);
}

/**
 * @brief enlil3DVar::asFloat
 * @param extents
 * @param nblk
 * @return
 */
QVector<float> enlil3DVar::asFloat(const int extents[], int nblk)
{
    enlilExtent X,Y,Z;
    X.first = extents[0];
    X.second = extents[1];
    Y.first = extents[2];
    Y.second = extents[3];
    Z.first = extents[4];
    Z.second = extents[5];

    return this->asFloat(X,Y,Z,nblk);
}

/**
 * @brief enlil3DVar::asFloat
 * @param X
 * @param Y
 * @param Z
 * @param nblk
 * @return
 */
QVector<float> enlil3DVar::asFloat(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk)
{
    QVector<QVariant> *raw = this->_getData(X,Y,Z, nblk);
    QVector<float> asFloat;

    if(!raw)
    {
        std::cerr << "ERROR: Failed to get Variable Data for " << qPrintable(this->__varLongName) << std::endl;
        exit(1);
    }

    int count = raw->count();

    for(int x = 0; x < count; x++)
    {
        bool status;
        float value = raw[0][x].toFloat(&status);
        if(!status)
        {
            std::cerr << "FAILURE TO CONVERT TO FLOAT." << std::endl;
        }
        else
        {
            asFloat.push_back(value);
        }
    }

    delete raw;
    return asFloat;
}

/**
 * @brief enlil3DVar::asFloat
 * @param nblk
 * @return
 */
QVector<float> enlil3DVar::asFloat(QVector<enlilExtent> extents, int nblk)
{
    return this->asFloat(extents[0],extents[1],extents[2],nblk);

}

/**
 * @brief enlil3DVar::asInt64
 * @param nblk
 * @return
 */
QVector<qint64> enlil3DVar::asInt64(int nblk)
{
    enlilExtent X = this->__wholeExtents["n1"];
    enlilExtent Y = this->__wholeExtents["n2"];
    enlilExtent Z = this->__wholeExtents["n3"];

    return this->asInt64(X,Y,Z, nblk);
}

/**
 * @brief enlil3DVar::asInt64
 * @param extents
 * @param nblk
 * @return
 */
QVector<qint64> enlil3DVar::asInt64(QVector<qint64> extents, int nblk)
{
    enlilExtent X,Y,Z;
    X.first = extents[0];
    X.second = extents[1];
    Y.first = extents[2];
    Y.second = extents[3];
    Z.first = extents[4];
    Z.second = extents[5];

    return this->asInt64(X,Y,Z,nblk);
}

/**
 * @brief enlil3DVar::asInt64
 * @param extents
 * @param nblk
 * @return
 */
QVector<qint64> enlil3DVar::asInt64(const int extents[], int nblk)
{
    enlilExtent X,Y,Z;
    X.first = extents[0];
    X.second = extents[1];
    Y.first = extents[2];
    Y.second = extents[3];
    Z.first = extents[4];
    Z.second = extents[5];

    return this->asInt64(X,Y,Z,nblk);
}

/**
 * @brief enlil3DVar::asInt64
 * @param X
 * @param Y
 * @param Z
 * @param nblk
 * @return
 */
QVector<qint64> enlil3DVar::asInt64(enlilExtent X, enlilExtent Y, enlilExtent Z, int nblk)
{
    QVector<QVariant> *raw = this->_getData(X,Y,Z, nblk);
    QVector<qint64> asInt64;

    if(!raw)
    {
        std::cerr << "ERROR: Failed to get Variable Data for " << qPrintable(this->__varLongName) << std::endl;
        exit(1);
    }

    int count = raw->count();

    for(int x = 0; x < count; x++)
    {
        bool status;
        qint64 value = raw[0][x].toLongLong(&status);
        if(!status)
        {
            std::cerr << "FAILURE TO CONVERT TO INT64." << std::endl;
        }
        else
        {
            asInt64.push_back(value);
        }
    }

    delete raw;
    return asInt64;
}

/**
 * @brief enlil3DVar::asInt64
 * @param nblk
 * @return
 */
QVector<qint64> enlil3DVar::asInt64(QVector<enlilExtent> extents, int nblk)
{
    return this->asInt64(extents[0],extents[1],extents[2],nblk);

}


/**
 * @brief enlil3DVar::getData
 * @param X
 * @param Y
 * @param Z
 * @param nblk
 * @return
 */
QVector<QVariant> *enlil3DVar::_getVariantData(NcVar* var, int length, size_t counts[], long startLoc[])
{
    QVector<QVariant> * data = new QVector<QVariant>;

    long readSize = 1;

    for(int loop = 0; loop < length; loop++)
    {
        if(counts[loop] > 0) readSize *= counts[loop];
    }

    //load the data
    switch(this->__type)
    {
    case ncNoType:
    {
        break;
    }
    case ncByte:
    {
        ncbyte *newData = new ncbyte[readSize];
        var->set_cur(startLoc);
        var->get(newData,counts);

        for(int loop = 0; loop < readSize; loop++)
        {
            //put the data in the data array
            data->push_back(QVariant(newData[loop]));
        }

        //clean up temp memory
        delete newData;

        break;
    }
    case ncChar:
    {
        char *newData = new char[readSize];
        var->set_cur(startLoc);
        var->get(newData,counts);

        for(int loop = 0; loop < readSize; loop++)
        {
            //put the data in the data array
            data->push_back(QVariant(newData[loop]));
        }

        //clean up temp memory
        delete newData;
        break;
    }
    case ncShort:
    {
        short *newData = new short[readSize];
        var->set_cur(startLoc);
        var->get(newData,counts);

        for(int loop = 0; loop < readSize; loop++)
        {
            //put the data in the data array
            data->push_back(QVariant(newData[loop]));
        }

        //clean up temp memory
        delete newData;
        break;
    }
    case ncInt:
    {
        int *newData = new int[readSize];
        var->set_cur(startLoc);
        var->get(newData,counts);

        for(int loop = 0; loop < readSize; loop++)
        {
            //put the data in the data array
            data->push_back(QVariant(newData[loop]));
        }

        //clean up temp memory
        delete newData;
        break;
    }
    case ncFloat:
    {
        float *newData = new float[readSize];
        var->set_cur(startLoc);
        var->get(newData,counts);

        for(int loop = 0; loop < readSize; loop++)
        {
            //put the data in the data array
            data->push_back(QVariant(newData[loop]));
        }

        //clean up temp memory
        delete newData;
        break;
    }
    case ncDouble:
    {
        double *newData = new double[readSize];
        var->set_cur(startLoc);
        var->get(newData,counts);

        for(int loop = 0; loop < readSize; loop++)
        {
            //put the data in the data array
            data->push_back(QVariant(newData[loop]));
        }

        //clean up temp memory
        delete newData;
        break;
    }
    default:
    {
        break;
    }

    }

    return data;
}

QVector<QVariant> *enlil3DVar::_getData(enlilExtent N1, enlilExtent N2, enlilExtent N3, int nblk)
{

    size_t *counts = NULL;
    long *startLoc = NULL;
    QVector<QVariant> *data = NULL;
    enlilExtent *curExtent = NULL;


    int numDims = this->__dims.count();

    counts = new size_t[numDims];
    startLoc = new long[numDims];

    //get pointer to variable data
    NcVar* current = this->__parent->_file->get_var(qPrintable(this->__varName));

    switch(numDims)
    {
    case 0:
    {
        //scalar value
        if(this->__recordCount > 1)
        {
            std::cerr << "Found more than one record... there should only be one!" << std::endl;
            break;
        }
        size_t localCounts = 1;
        long localStartLoc = 0;

        data = this->_getVariantData(current, 1,  &localCounts, &localStartLoc);

        break;
    }
    case 1:
    {
        //1-D value with no blocks.


        if(this->__dims.contains("n1"))
        {
            curExtent = &N1;
        }
        else if(this->__dims.contains("n2"))
        {
            curExtent = &N2;
        }
        else if(this->__dims.contains("n3"))
        {
            curExtent = &N3;
        }
        else
        {
            std::cerr << "I am curflumoxed.  I have not idea what to do with this type of dimension..." << std::endl;
            //don't know what to do here...
        }

        //build the call
        counts[0] = curExtent->second - curExtent->first + 1;
        startLoc[0] = curExtent->first;

        data = this->_getVariantData(current, 1, counts, startLoc);

        break;
    }
    case 2:
    {
        //1-D value with blocks or 2-D value with no blocks;
        if(this->__recordCount > 1)
        {
            if(this->__dims.contains("n1"))
            {
                curExtent = &N1;
            }
            else if(this->__dims.contains("n2"))
            {
                curExtent = &N2;
            }
            else if(this->__dims.contains("n3"))
            {
                curExtent = &N3;
            }
            else
            {
                std::cerr << "I am curflumoxed.  I have not idea what to do with this type of dimension..." << std::endl;
                //don't know what to do here...
            }

            //this is a 1-D file with blocks
            startLoc[0] = nblk;
            startLoc[1] = curExtent->first;

            counts[0]=1;
            counts[1]= curExtent->second - curExtent->first + 1;

            data = this->_getVariantData(current, 2, counts, startLoc);

        }
        else
        {
            std::cerr << __FUNCTION__ <<  " 2-D non-block read not yet implemented." << std::endl;
            //this is a 2-D file with no blocks
        }

        break;
    }
    case 3:
    {
        //2-D value with blocks, or 3-D value with no blocks;
        if(this->__recordCount > 1)
        {
            std::cerr << __FUNCTION__ <<  " 2-D block read not yet implemented." << std::endl;
            //this is a 2-D file with Blocks
        }
        else
        {
            //Enlil files encode 3d data in n3, n2, n1
            //this is a 3-D file with no Blocks
            startLoc[0] = N3.first;
            startLoc[1] = N2.first;
            startLoc[2] = N1.first;

            counts[0] = N3.second - N3.first +1;
            counts[1] = N2.second - N2.first +1;
            counts[2] = N1.second - N1.first +1;

            data = this->_getVariantData(current, 3, counts, startLoc);
        }

        break;
    }
    case 4:
    {
        if(this->__recordCount < 1)
        {
            std::cerr << "We don't know how to handle a 4-D file for Enlil..." << std::endl;
            break;
        }
        //this is a 3-D file with blocks
        //Enlil files encode the data in nblk, n3, n2, n1
        startLoc[0] = nblk;
        startLoc[1] = N3.first;
        startLoc[2] = N2.first;
        startLoc[3] = N1.first;

        counts[0] = 1;
        counts[1] = N3.second - N3.first +1;
        counts[2] = N2.second - N2.first +1;
        counts[3] = N1.second - N1.first +1;

        data = this->_getVariantData(current, 4, counts, startLoc);

        break;
    }
    default:
        std::cerr << "No known vars of dimension " << numDims << std::endl;
        break;

    }

    return data;

}

/**
 * @brief enlilVar::recordCount
 * @return
 */
qint64 enlil3DVar::recordCount()
{
    return this->__recordCount;
}

/**
 * @brief enlil3DVar::getExtent
 * @param name
 * @return
 */
enlilExtent enlil3DVar::getExtent(const char *name)
{
    if(this->__wholeExtents.contains(QString(name)))
    {
        return this->__wholeExtents[QString(name)];
    }
    else
    {
        std::cerr << "ERROR: Extent not found" << std::endl;
        enlilExtent voidExtent;
        voidExtent.first = 0;
        voidExtent.second = 0;
        return voidExtent;
    }

}



/**
 * @brief enlil3DVar::_loadMetaData
 */
void enlil3DVar::_loadMetaData()
{
    int numDims = 0;
    int numAtts = 0;

    char *name = this->__varName.toAscii().data();
    QMap<QString, NcDim*> dimMap;

    NcVar* variable = this->__parent->_file->get_var(name);

    this->__type = variable->type();

    numDims = variable->num_dims();
    numAtts = variable->num_atts();

    //get all of the dimension records
    for(int loop = 0; loop < numDims; loop++)
    {
        NcDim* dim1 = variable->get_dim(loop);
        if(!dim1) continue;
        dimMap[QString(dim1->name())] = dim1;

        enlilExtent extent;
        extent.first = 0;
        extent.second = dimMap[QString(dim1->name())]->size() - 1;

        this->__dims[QString(dim1->name())] = extent.second + 1;
        this->__wholeExtents[QString(dim1->name())] = extent;
    }

    //see if we have multiple records in the file
    if(dimMap.contains(QString("nblk")))
    {
        //we have a dimension that gives us the number of records
        this->__recordCount = dimMap[QString("nblk")]->size();
    }
    else
    {
        //if there is no nblk, the record count is 1
        this->__recordCount = 1;
    }

    //load the attributes.
    for(int loop = 0; loop < numAtts; loop++)
    {
        NcAtt* attribute = variable->get_att(loop);
        NcType ValueType = attribute->type();

        QVariant value;

        switch(ValueType)
        {
        case ncNoType:
            std::cerr << "ERROR: Unknown Attribute Type. Cannot Store." << std::endl;
            break;

        case ncByte:
            value = QVariant(attribute->as_ncbyte(0));
            break;

        case ncChar:
            value = QVariant(attribute->as_string(0));
            break;

        case ncShort:
            value = QVariant(attribute->as_short(0));
            break;

        case ncInt:
            value = QVariant(attribute->as_int(0));
            break;

            //case ncLong:  //This is depricated, and returns same value as ncInt
            //    break;

        case ncFloat:
            value = QVariant(attribute->as_float(0));
            break;

        case ncDouble:
            value = QVariant(attribute->as_double(0));
            break;

        default:
            std::cerr << "ERROR: Unknown Attribute Type. Cannot Store." << std::endl;
            break;
        }


        enlilAtt* newAtt = new enlilAtt(ValueType, QString(attribute->name()), value);

        this->__atts[QString(attribute->name())] = newAtt;

        //clean up memory
        delete attribute;
    }

    if(this->__atts.contains(QString("long_name")))
    {
        //add long name
        this->__varLongName = this->__atts[QString("long_name")]->getValue().toString();
    }

    if(this->__atts.contains(QString("units")))
    {
        //add units
        this->__units = this->__atts[QString("units")]->getValue().toString();
    }

}


