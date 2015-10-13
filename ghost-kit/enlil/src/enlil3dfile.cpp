#include "enlil3dfile.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <vtkMath.h>
#include "ltrDateTime.h"

enlil3DFile::enlil3DFile(const char *FileName, double scaleFactor)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //setup the object
    this->setFileName(FileName);
    this->__scale_factor = scaleFactor;
    this->outputRaw = false;
    this->selectOutput(0);

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
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

}

void enlil3DFile::_initializeFiles()
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //open the file
//    std::cout << "Opening file: " << qPrintable(this->fileName) << std::endl;
    this->file = new NcFile(qPrintable(this->fileName));

//    std::cout << "File Opened..." << std::endl << std::flush;


    if(!this->file->is_valid())
    {
        std::cerr << "Error loading file " << qPrintable(this->fileName) << std::endl;
        return;
    }

    //get number of entries
    NcDim *dims[3];
    dims[0] = this->file->get_dim("n1");
    dims[1] = this->file->get_dim("n2");
    dims[2] = this->file->get_dim("n3");

    if(dims[0]->is_valid() && dims[1]->is_valid() && dims[2]->is_valid())
    {
        this->dims[0] = dims[0]->size();
        this->dims[1] = dims[1]->size();
        this->dims[2] = dims[2]->size();
    }

//    std::cout << "Dims retrieved..." << this->dims[0] << "," << this->dims[1] << "," << this->dims[2] << std::endl;
//    std::cout << "Getting Var and Att lists..." << std::endl;

    //get list of variables
    QStringList vars = this->_getVaribleList();
    QStringList atts = this->_getAttributeList();

//    std::cout << "Vars and Atts retrieved..." << std::endl;

    //process atts
    for(int x=0; x < atts.size(); x++)
    {
        this->_loadFileAttribute(atts[x]);
    }

    //process vars
    //TODO: LOAD TIME FIRST. Then links to methods for getting data
    //TODO: create method for loading the actual data.
    if(vars.contains(QString("TIME")))
    {
        this->_loadVariable(QString("TIME"));
        this->TIME = this->_variablesRaw["TIME"][0];

//        std::cout << "TIME: " << this->TIME << std::endl << std::flush;
    }

        for(int x=0; x < vars.size(); x++)
        {
//            std::cout << "Loading Var: " << vars[x].toAscii().data() << std::endl;
            this->_loadVariable(vars[x]);

//            std::cout << "Loading Variable Attributes..." << std::endl << std::flush;
            QStringList varAtts = this->_getAttListForVar(vars[x]);

//                    std::cout << "reading attributes" << std::endl;
                    for(int y=0; y < varAtts.size();y++)
                    {
//                        std::cout << "getting var attribute " << varAtts[y].toAscii().data() << std::flush << std::endl;
                        this->_loadAttFromVar(vars[x], varAtts[y]);
                    }
        }



    this->file->close();


}

void enlil3DFile::setFileName(const char *FileName)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    this->fileName = QString(FileName);
}

void enlil3DFile::setName(const char *name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    this->name = QString(name);
}

void enlil3DFile::addUnitConversion(const char *baseUnits, const char *newUnits, double divisor)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    this->_addConversion(QString(baseUnits), QString(newUnits), divisor);
}

QString enlil3DFile::getFileName()
{
//    std::cout << "Enlil3d Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->fileName;

}

QString enlil3DFile::getName()
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->name;
}

int enlil3DFile::getDims(int xyz)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->dims[xyz];

}

int enlil3DFile::get3Dcount()
{
    return this->dims[0]*this->dims[1]*this->dims[2];
}

QStringList enlil3DFile::getVarNames()
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    if(!this->varOutput)
    {
//        std::cerr << "Var Output is not defined.  Fix Me!" << std::endl << std::flush;
        exit(EXIT_FAILURE);
    }

        return this->varOutput->keys();

}

QStringList enlil3DFile::getFileAttributeNames()
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->fileAttributeData.keys();

}

bool enlil3DFile::hasUnits(const char *name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    if(this->varUnitsOutput->contains(QString(name))) return true;
    else return false;
}

bool enlil3DFile::hasUnits(QString name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return hasUnits(name.toAscii().data());
}

QVector<double> enlil3DFile::getVar(const char *name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->varOutput->operator [](QString(name));

}

QString enlil3DFile::getVarUnits(const char *name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->varUnitsOutput->operator [](QString(name));

}

QString enlil3DFile::getVarLongName(const char *name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->longNamesOutput->operator [](QString(name));

}

qint64 enlil3DFile::getNumberOfVars()
{
    return qint64(this->getVarNames().count());
}

QVariant enlil3DFile::getFileAttribute(const char *name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return this->fileAttributeData[QString(name)];

}

int enlil3DFile::getNumberOfFileAttributes()
{
    return this->fileAttributeData.count();
}

void enlil3DFile::selectOutput(int version)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    switch(version)
    {
    case 0:
        //std::cout << "Using Raw Values..." << std::endl;
        this->varOutput = &_variablesRaw;
        this->varUnitsOutput = &_varUnitsRaw;
        this->longNamesOutput = &_longNamesRaw;
        break;

    case 1:
        // std::cout << "Using Processed Values..." << std::endl;
        this->varOutput = &_variablesProcessed;
        this->varUnitsOutput = &_varUnitsProcessed;
        this->longNamesOutput = &_longNamesProcessed;
        break;

    default:
        break;
    }
}

double enlil3DFile::getMJD() const
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return MJD;
}



void enlil3DFile::_loadVariable(QString name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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

void enlil3DFile::_loadFileAttribute(QString name)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;
//    std::cout << "Name: " << name.toAscii().data() << std::endl << std::flush;

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

    this->fileAttributeData[name] = variant;

}

QStringList enlil3DFile::_getVaribleList()
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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
    //TODO: There should be a better way to handle Varaiable Attributes
    //TODO: Find way to handle any variable attribute.
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
    else
    {
        std::cerr << "Unknown Variable Attribute.  Consider revising the reader." << std::endl;
    }
}

QStringList enlil3DFile::_getAttListForVar(QString varName)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    QStringList values;
    QStringList variables = this->getVarNames();

    // IF data not available, return empty values
    if(!variables.contains(varName))
    {
        std::cerr << "Variable " << varName.toAscii().data()
                  << " Cannot be found for loading attributes."
                  << std::endl;
        return values;
    }

    NcVar* var = this->file->get_var(qPrintable(varName));
    qint64 numAtts = var->num_atts();

//    std::cout << "Number of Attributes for " << qPrintable(varName) << ": " << numAtts << std::endl;

    for(int x=0; x<numAtts; x++)
    {
//        std::cout << "Getting Attribute: " << x << std::endl;

        NcAtt* att = var->get_att(x);
        if(att->is_valid()) values.push_back(QString(att->name()));

//        std::cout << "Name: " << att->name() << std::flush << std::endl;
    }

    //std::cout << "Returning to calling function..." << std::endl;
    return values;
}

void enlil3DFile::_processLocation()
{
        std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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

            int numPoints = this->get3Dcount();

            //TODO: This needs to be a three way loop.
            //FIXME: MUST FIX THIS LOOP NEXT
            for(int a = 0; a < numPoints; a++)
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
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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
        //        for(int y=0; y < this->stepCount; y++)
        //        {
        //            //data origin
        //            rtpOrigin[0] = R[y];
        //            rtpOrigin[1] = T[y];
        //            rtpOrigin[2] = P[y];

        //            rtp[0] = DR[y];
        //            rtp[1] = DT[y];
        //            rtp[2] = DP[y];

        //            //convert
        //            xyz = this->_sphere2Cart(rtp, rtpOrigin);

        //            DRc.push_back(DR[y]/divisor);
        //            DTc.push_back(DT[y]/divisor);
        //            DPc.push_back(DP[y]/divisor);

        //            X.push_back(xyz[0]/divisor);
        //            Y.push_back(xyz[1]/divisor);
        //            Z.push_back(xyz[2]/divisor);

        //            delete [] xyz;
        //        }

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
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    if(!this->_variablesRaw.keys().contains("TIME"))
    {
//        std::cerr << "Missing Variable TIME." << std::endl;
//        exit(1);
        return; //do not process if doesn't have "TIME"
    }

    if(!this->getFileAttributeNames().contains("version"))
    {
//        std::cerr << "Missing Versions..." << std::endl;
        exit(1);
        return; //make sure there is a version
    }

    double version = this->getFileAttribute("version").toDouble(); //need to know the version of the run
//    std::cout << "Version: " << version << std::endl;

    // The version of the file dictates the variable name for the reference date
    if(version < 2.8)
    {
        this->runDataString = QString("refdate_mjd");
    }
    else    // variable name changed in version 2.8
    {
        this->runDataString = QString("rundate_mjd");
    }

    //If the file does not contain the proper version, throw an error.
    if(!this->getFileAttributeNames().contains(this->runDataString))
    {
        std::cerr << "ERROR: Enlil Version " << version << " Does not contain the proper reference date variable for this reader.  Please submit a bug report." << std::endl;
        return; //do not process
    }

    //Get the reference date
    double refmjd = this->getFileAttribute(this->runDataString.toAscii().data()).toDouble(0);

    //TODO: Get time ... this is a variable read, so I need to figure out how to dynamically do this...
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
    this->MJD = mjd[0];
}

void enlil3DFile::_addConversion(QString baseUnits, QString newUnits, double divisor)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    this->_convMap[baseUnits] = qMakePair(newUnits, divisor);
    this->_processSphericalVectors();
    this->_processScalars();
}

QPair<QString, double> enlil3DFile::_getConvDivForVar(QString var)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

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
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //calculate
    double* xyz = new double[3];
    xyz[0] = rtp[0] * sin(rtp[1]) * cos(rtp[2]);
    xyz[1] = rtp[0] * sin(rtp[1]) * sin(rtp[2]);
    xyz[2] = rtp[0] * cos(rtp[1]);

    return xyz;
}

double *enlil3DFile::_sphere2Cart(const double rtp[], const double rtpOrigin[])
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    double *vector = new double[3];

    vector[0] = (rtp[0] * sin(rtpOrigin[1]) * cos(rtpOrigin[2])) + (rtp[1] * cos(rtpOrigin[1]) * cos(rtpOrigin[2])) + (-1.0*rtp[2] * sin(rtpOrigin[2]));
    vector[1] = (rtp[0] * sin(rtpOrigin[1]) * sin(rtpOrigin[2])) + (rtp[1] * cos(rtpOrigin[1]) * sin(rtpOrigin[2])) + (rtp[2] * cos(rtpOrigin[2]));
    vector[2] = (rtp[0] * cos(rtpOrigin[1])) + (-1.0*rtp[1] * sin(rtpOrigin[1]));

    return vector;
}

double enlil3DFile::getScale_factor() const
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    return __scale_factor;
}

void enlil3DFile::setScale_factor(double scale_factor)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    //TODO: CRITICAL: Must re-calculate after scale-factor is set.
    __scale_factor = scale_factor;
}

double enlil3DFile::getMax(QVector<double> vector)
{
//    std::cout << "Function: " << __FUNCTION__ << std::endl << std::flush;

    qSort(vector);
    double max = vector.last();

    return max;
}

