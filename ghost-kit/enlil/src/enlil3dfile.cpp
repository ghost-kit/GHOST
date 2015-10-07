#include "enlil3dfile.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <vtkMath.h>
#include "ltrDateTime.h"

enlil3DFile::enlil3DFile(const char *FileName, double scaleFactor)
{
    //setup the object
    this->setFileName(FileName);
    this->__scale_factor = scaleFactor;
    this->outputRaw = false;

    //load the data from the file
    this->_initializeFiles();

    //process location
    this->_processLocation();

    //process Time
    this->_processTime();

    //process spherical vectors
    this->_processSphericalVectors();

    //process scalars
    this->_processScalars();
}

enlil3DFile::~enlil3DFile()
{

}

void enlil3DFile::_initializeFiles()
{

    //open the file
    std::cout << "Opening file: " << qPrintable(this->fileName) << std::endl;
    this->file = new NcFile(qPrintable(this->fileName));

    if(!this->file->is_valid())
    {
        std::cerr << "Error loading file " << qPrintable(this->fileName) << std::endl;
        return;
    }

    //get number of entries
    NcDim *dims = this->file->get_dim("nevo");
    std::cout << "Dims retrieved..." << std::endl;
    if(dims->is_valid())
    {
        this->stepCount = dims->size();
        //std::cerr << "stepCount: " << this->stepCount << std::endl;
    }

    std::cout << "Getting Var and Att lists..." << std::endl;

    //get list of variables
    QStringList vars = this->_getVaribleList();
    QStringList atts = this->_getAttributeList();

    std::cout << "Vars and Atts retrieved..." << std::endl;

    //process vars
    for(int x=0; x < vars.size(); x++)
    {
        std::cout << "Loading Var: " << vars[x].toAscii().data() << std::endl;
        this->_loadVariable(vars[x]);

        QStringList varAtts = this->_getAttListForVar(vars[x]);

                //std::cout << "reading attributes" << std::endl;
                for(int y=0; y < varAtts.size();y++)
                {
                    std::cout << "getting var attributes.." << std::flush << std::endl;
                    //get specified vars if they exist
                    if(varAtts[y] == QString("units") || varAtts[y] == QString("long_name"));
                        this->_loadAttFromVar(vars[x], varAtts[y]);
                }
    }

    //process atts
    for(int x=0; x < atts.size(); x++)
    {
        this->_loadMetaData(atts[x]);
    }

    this->file->close();


}

void enlil3DFile::setFileName(const char *FileName)
{
    this->fileName = QString(FileName);
}

void enlil3DFile::setName(const char *name)
{
    this->name = QString(name);
}

void enlil3DFile::addUnitConversion(const char *baseUnits, const char *newUnits, double divisor)
{
    this->_addConversion(QString(baseUnits), QString(newUnits), divisor);
}

QString enlil3DFile::getFileName()
{
    return this->fileName;

}

QString enlil3DFile::getName()
{
    return this->name;
}

qint64 enlil3DFile::getStepCount()
{
    return this->stepCount;

}

QStringList enlil3DFile::getVarNames()
{
    return this->varOutput.keys();
}

QStringList enlil3DFile::getMeataDataNames()
{
    return this->fileMetaData.keys();

}

bool enlil3DFile::hasUnits(const char *name)
{
    if(this->varUnitsOutput.contains(QString(name))) return true;
    else return false;
}

bool enlil3DFile::hasUnits(QString name)
{
    return hasUnits(name.toAscii().data());
}

QVector<double> enlil3DFile::getVar(const char *name)
{
    return this->varOutput[QString(name)];

}

QString enlil3DFile::getVarUnits(const char *name)
{
    return this->varUnitsOutput[QString(name)];

}

QString enlil3DFile::getVarLongName(const char *name)
{
    return this->longNamesOutput[QString(name)];

}

QVariant enlil3DFile::getMetaData(const char *name)
{
    return this->fileMetaData[QString(name)];

}

void enlil3DFile::selectOutput(int version)
{
    switch(version)
    {
    case 0:
        //std::cout << "Using Raw Values..." << std::endl;
        this->varOutput = _variablesRaw;
        this->varUnitsOutput = _varUnitsRaw;
        this->longNamesOutput = _longNamesRaw;
        break;

    case 1:
       // std::cout << "Using Processed Values..." << std::endl;
        this->varOutput = _variablesProcessed;
        this->varUnitsOutput = _varUnitsProcessed;
        this->longNamesOutput = _longNamesProcessed;
        break;

    default:
        break;
    }
}



void enlil3DFile::_loadVariable(QString name)
{
    long readStart[2] = {0,0};

    NcVar *variable = this->file->get_var(name.toAscii().data());
    variable->set_cur(readStart);


    //get the values from file
    NcValues* values = variable->values();
    int numElem = values->num();

    //create a data structure for storage
    QVector<double> qVals;
    for(int x=0; x < numElem; x++)
    {
        qVals.push_back(values->as_double(x));
    }

    //save the variable
    this->_variablesRaw[name] = qVals;

    //free the memory
    delete values;
}

void enlil3DFile::_loadMetaData(QString name)
{
    NcAtt *attribute = this->file->get_att(name.toAscii().data());
    NcType attType = attribute->type();
    QVariant variant;

    switch(attType)
    {
    case ncChar:
    {
        QVariant value(QString(attribute->as_string(0)));
        variant = value;
        break;
    }

    case ncInt:
    {
        QVariant value(attribute->as_int(0));
        variant = value;
        break;
    }

    case ncFloat:
    {
        QVariant value(attribute->as_float(0));
        variant = value;
        break;
    }

    case ncDouble:
    {
        QVariant value(attribute->as_double(0));
        variant = value;
        break;
    }

    default:
        std::cerr << "Requested conversion from unknown type" << std::endl;
        break;
    }

    this->fileMetaData[name] = variant;

}

QStringList enlil3DFile::_getVaribleList()
{
    qint64 numVars = this->file->num_vars();
    QStringList values;

    for(int x=0; x<numVars; x++)
    {
        NcVar* var = this->file->get_var(x);
        values.push_back(QString(var->name()));
    }

    return values;
}

QStringList enlil3DFile::_getAttributeList()
{
    qint64 numAtts = this->file->num_atts();
    QStringList values;

    for(int x=0; x<numAtts; x++)
    {
        NcAtt* att = this->file->get_att(x);
        values.push_back(QString(att->name()));
    }

    return values;
}

void enlil3DFile::_loadAttFromVar(QString VarName, QString AttName)
{
    QString value;
    NcVar* var = this->file->get_var(VarName.toAscii().data());
    NcAtt* att = var->get_att(AttName.toAscii().data());

    value = QString(att->as_string(0));

    if(AttName == "units")
    {
        this->_varUnitsRaw[VarName] = value;
    }
    else if(AttName == "long_name")
    {
        this->_longNamesRaw[VarName] = value;
    }
}

QStringList enlil3DFile::_getAttListForVar(QString varName)
{
    QStringList values;
    QStringList variables = this->_variablesRaw.keys();

    // IF data not available, return empty values
    if(!variables.contains(varName)) return values;

    NcVar* var = this->file->get_var(qPrintable(varName));
    qint64 numAtts = var->num_atts();

    for(int x=0; x<numAtts; x++)
    {
        //std::cout << "Getting Attribute: " << x << std::endl;

        NcAtt* att = var->get_att(x);
        if(att->is_valid()) values.push_back(QString(att->name()));

       // std::cout << "Name: " << att->name() << std::flush << std::endl;
    }

    //std::cout << "Returning to calling function..." << std::endl;
    return values;
}

void enlil3DFile::_processLocation()
{
    //check existance of the position values
    QStringList vars = this->_variablesRaw.keys();

    if(vars.contains("X1") && vars.contains("X2") && vars.contains("X3"))
    {
        QVector<double> x1 = this->_variablesRaw["X1"];
        QVector<double> x2 = this->_variablesRaw["X2"];
        QVector<double> x3 = this->_variablesRaw["X3"];

        QVector<double> X;
        QVector<double> Y;
        QVector<double> Z;

        QVector<double> R;
        QVector<double> T;
        QVector<double> P;

        double* rtp = new double[3];
        double* xyz = NULL;

        for(int a = 0; a < this->stepCount; a++)
        {

            rtp[0] = x1[a];
            rtp[1] = x2[a];
            rtp[2] = x3[a];

            xyz = this->_gridSphere2Cart(rtp);

            R.push_back(rtp[0]/this->__scale_factor);
            T.push_back(rtp[1]);
            P.push_back(rtp[2]);

            X.push_back(xyz[0]/this->__scale_factor);
            Y.push_back(xyz[1]/this->__scale_factor);
            Z.push_back(xyz[2]/this->__scale_factor);

            delete [] xyz;

        }

        delete [] rtp;

        //save converted results
        this->_variablesProcessed["pos_X"] = X;
        this->_variablesProcessed["pos_Y"] = Y;
        this->_variablesProcessed["pos_Z"] = Z;

        //save processed data (RTP)
        this->_variablesProcessed[QString("pos_R")] = R;
        this->_variablesProcessed[QString("pos_T")] = T;
        this->_variablesProcessed[QString("pos_P")] = P;
    }

}

void enlil3DFile::_processSphericalVectors()
{
    double divisor = 1;
    QString newUnits;

    QStringList vars = this->_variablesRaw.keys();
    QStringList vectors;
    QString var;

    for(int x = 0; x < vars.size(); x++)
    {
        //look for vectors of the format ?1, ?2, ?3
        var = vars[x];
        if(var.endsWith("1") || var.endsWith("2") || var.endsWith("3"))
        {
            var.remove("1");
            var.remove("2");
            var.remove("3");

            vectors.push_back(var);
        }
    }
    vectors.removeDuplicates();

    //if we don't have position data, we cannot convert.
    if(!vectors.contains("X")) return;

    //get the location data
    QVector<double> R = this->_variablesRaw["X1"];
    QVector<double> T = this->_variablesRaw["X2"];
    QVector<double> P = this->_variablesRaw["X3"];

    //process the remaining data
    for(int x = 0; x < vectors.count(); x++)
    {
        //skip the location variable
        if(vectors[x] == "X") continue;

        //get the units for the FIRST part of the vector... so as not to get radians
        QPair<QString, double> conversion = this->_getConvDivForVar(vectors[x]+"1");
        divisor = conversion.second;
        newUnits = conversion.first;

        //std::cout << "Divisor: " << divisor << std::endl;
        //std::cout << "Units:   " << qPrintable(newUnits) << std::endl;

        //get variables to process
        QVector<double> DR = this->_variablesRaw[(QString(vectors[x] + "1").toAscii().data())];
        QVector<double> DT = this->_variablesRaw[(QString(vectors[x] + "2").toAscii().data())];
        QVector<double> DP = this->_variablesRaw[(QString(vectors[x] + "3").toAscii().data())];

        QVector<double> DRc;
        QVector<double> DTc;
        QVector<double> DPc;

        QVector<double> X;
        QVector<double> Y;
        QVector<double> Z;

        double* rtp = new double[3];
        double* rtpOrigin = new double[3];
        double* xyz = NULL;

        //process
        for(int y=0; y < this->stepCount; y++)
        {
            //data origin
            rtpOrigin[0] = R[y];
            rtpOrigin[1] = T[y];
            rtpOrigin[2] = P[y];

            rtp[0] = DR[y];
            rtp[1] = DT[y];
            rtp[2] = DP[y];

            //convert
            xyz = this->_sphere2Cart(rtp, rtpOrigin);

            DRc.push_back(DR[y]/divisor);
            DTc.push_back(DT[y]/divisor);
            DPc.push_back(DP[y]/divisor);

            X.push_back(xyz[0]/divisor);
            Y.push_back(xyz[1]/divisor);
            Z.push_back(xyz[2]/divisor);

            delete [] xyz;
        }

        delete [] rtp;
        delete [] rtpOrigin;

        //save processed data (XYZ)
        this->_variablesProcessed[QString(vectors[x]+"_X")] = X;
        this->_variablesProcessed[QString(vectors[x]+"_Y")] = Y;
        this->_variablesProcessed[QString(vectors[x]+"_Z")] = Z;

        this->_varUnitsProcessed[QString(vectors[x]+"_X")] = newUnits;
        this->_varUnitsProcessed[QString(vectors[x]+"_Y")] = newUnits;
        this->_varUnitsProcessed[QString(vectors[x]+"_Z")] = newUnits;


        //save processed data (RTP)
        this->_variablesProcessed[QString(vectors[x]+"_R")] = DRc;
        this->_variablesProcessed[QString(vectors[x]+"_T")] = DTc;
        this->_variablesProcessed[QString(vectors[x]+"_P")] = DPc;

        this->_varUnitsProcessed[QString(vectors[x]+"_R")] = newUnits;
        this->_varUnitsProcessed[QString(vectors[x]+"_T")] = newUnits;
        this->_varUnitsProcessed[QString(vectors[x]+"_P")] = newUnits;

    }

}

void enlil3DFile::_processScalars()
{
    double divisor=1;
    QString newUnits;
    QStringList vars = this->_variablesRaw.keys();
    QStringList scalars;
    QString var;

    //get list of scalars
    for(int x = 0; x < vars.size(); x++)
    {
        var = vars[x];

        //get the scalar name
        if(var.endsWith("1") || var.endsWith("2") || var.endsWith("3")) continue;

        scalars.push_back(var);
    }

    //convert as necessary
    for(int x = 0; x < scalars.count(); x++)
    {
        QVector<double> oldValues = this->_variablesRaw[(scalars[x].toAscii().data())];
        QVector<double> newValues;

        QPair<QString, double> conversion = this->_getConvDivForVar(scalars[x]);
        divisor = conversion.second;
        newUnits = conversion.first;

        for(int y = 0; y < oldValues.count(); y++)
        {
            newValues.push_back(oldValues[y]/divisor);
        }


        this->_variablesProcessed[scalars[x]] = newValues;
        this->_varUnitsProcessed[scalars[x]] = newUnits;
        this->_longNamesProcessed[scalars[x]] = this->_longNamesRaw[scalars[x]];
    }

}

void enlil3DFile::_processTime()
{
    if(!this->_variablesRaw.keys().contains("TIME")) return; //do not process
    if(!this->getMeataDataNames().contains("rundate_mjd")) return; //do not process

    double refmjd = this->getMetaData("rundate_mjd").toDouble(0);

    QVector<double> timeVec = this->_variablesRaw[("TIME")];

    double timeMax = this->getMax(timeVec);
    if(timeMax > 3.15569e12)
    {
        std::cerr << "Time offset for last time step is more than 100,000 years from the reference time... you may want to check your model output... Cannot calculate a Modified Julian Date for " << qPrintable(this->fileName)<< std::endl;
        return;    //invalid times
    }

    DateTime refDate(refmjd);

    QVector<double> mjd;

    for(int x=0; x < timeVec.count(); x++)
    {
        DateTime newDate(refDate.getMJD());
        newDate.incrementSeconds(timeVec[x]);

        mjd.push_back(newDate.getMJD());

    }

    this->_variablesProcessed["MJD"] = mjd;
}

void enlil3DFile::_addConversion(QString baseUnits, QString newUnits, double divisor)
{
    this->_convMap[baseUnits] = qMakePair(newUnits, divisor);
    this->_processSphericalVectors();
    this->_processScalars();
}

QPair<QString, double> enlil3DFile::_getConvDivForVar(QString var)
{
    QString base;
    if(this->_varUnitsRaw.keys().contains(var))
    {
        base = this->_varUnitsRaw[var];
    }

    if(this->_convMap.keys().contains(base))
    {
        //if there is a conversion loaded, use it
        return _convMap[base];
    }
    else
    {
        //else use the original
        return qMakePair(base, 1.0);
    }

}

double *enlil3DFile::_gridSphere2Cart(const double rtp[])
{
    //calculate
    double* xyz = new double[3];
    xyz[0] = rtp[0] * sin(rtp[1]) * cos(rtp[2]);
    xyz[1] = rtp[0] * sin(rtp[1]) * sin(rtp[2]);
    xyz[2] = rtp[0] * cos(rtp[1]);

    return xyz;
}

double *enlil3DFile::_sphere2Cart(const double rtp[], const double rtpOrigin[])
{
    double *vector = new double[3];

    vector[0] = (rtp[0] * sin(rtpOrigin[1]) * cos(rtpOrigin[2])) + (rtp[1] * cos(rtpOrigin[1]) * cos(rtpOrigin[2])) + (-1.0*rtp[2] * sin(rtpOrigin[2]));
    vector[1] = (rtp[0] * sin(rtpOrigin[1]) * sin(rtpOrigin[2])) + (rtp[1] * cos(rtpOrigin[1]) * sin(rtpOrigin[2])) + (rtp[2] * cos(rtpOrigin[2]));
    vector[2] = (rtp[0] * cos(rtpOrigin[1])) + (-1.0*rtp[1] * sin(rtpOrigin[1]));

    return vector;
}

double enlil3DFile::getMax(QVector<double> vector)
{
    qSort(vector);
    double max = vector.last();

    return max;
}

