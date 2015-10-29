#include "enlil3Dvar.h"
#include "enlil3dfile.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <vtkMath.h>
#include "ltrDateTime.h"
#include "enlilAtt.h"

/**
 * @brief enlil3DFile::enlil3DFile
 * @param FileName
 * @param scaleFactor
 */
void enlil3DFile::__ResetFile()
{
    this->__initializeFiles();

    //process location
    this->__processGridLocations();

    //process Time
    this->__processTime();

    //process spherical vectors
    this->__processSphericalVectors();

    //process scalars
    this->__processScalars();
}

enlil3DFile::enlil3DFile(const char *FileName, double scaleFactor)
{
    //setup the object
    this->setFileName(FileName);
    this->_gridScaleFactor = scaleFactor;
    this->_convertData = false;
    this->_gridOutput = NULL;



    //load the data from the file
    __ResetFile();
}

/**
 * @brief enlil3DFile::~enlil3DFile
 */
enlil3DFile::~enlil3DFile()
{
    //FIXME: Need to manage all of the memory for the class


}


/**
 * @brief enlil3DFile::cleanAll
 */
void enlil3DFile::__cleanAll()
{
    delete this->_file;
    this->_MJD = 0;
    this->_TIME = 0;
    this->_name.clear();
    this->_enlil_version = 0;
    this->_gridScaleFactor = 1;
    this->_gridOutput = NULL;
    this->_gridPositionsCT.clear();
    this->_gridPositionsSP.clear();

    this->_fileAttributeData.clear();
    this->_convMap.clear();

}


/**
 * @brief enlil3DFile::getExtents
 * @return
 */
QMap<QString, enlilExtent> enlil3DFile::getWholeExtents()
{
    return this->_wholeExtents;
}



/**
 * @brief enlil3DFile::__initializeFiles
 */
void enlil3DFile::__initializeFiles()
{
    //open the file
    this->_file = new NcFile(qPrintable(this->_fileName));

    if(!this->_file->is_valid())
    {
        std::cerr << "Error loading file " << qPrintable(this->_fileName) << std::endl;
        return;
    }

    //get number of entries
    NcDim *dims[4];
    dims[0] = this->_file->get_dim("n1");
    dims[1] = this->_file->get_dim("n2");
    dims[2] = this->_file->get_dim("n3");
    dims[3] = this->_file->get_dim("nblk");


    if(dims[0] && dims[1] && dims[2])
    {
        this->_dims["n1"] = dims[0]->size();
        this->_dims["n2"] = dims[1]->size();
        this->_dims["n3"] = dims[2]->size();
        if(dims[3])
        {
            this->_dims["nblk"] = dims[3]->size();
        }
        else
        {
            this->_dims["nblk"] = 1;
        }
    }
    else
    {
        std::cerr << "ERROR: Dimensional Failure... " << std::endl;
        exit(EXIT_FAILURE);
    }




    //get list of variables
    QStringList vars = this->__getVaribleList();
    QStringList atts = this->__getAttributeList();

    //process atts
    for(int x=0; x < atts.size(); x++)
    {
        this->__loadFileAttribute(atts[x]);
    }


    //load Vars
    for(int x=0; x < vars.size(); x++)
    {
        this->__loadVariable(vars[x]);
    }



}

/**
 * @brief enlil3DFile::setFileName
 * @param FileName
 */
void enlil3DFile::setFileName(const char *FileName)
{
    this->_fileName = QString(FileName);
}

/**
 * @brief enlil3DFile::setName
 * @param name
 */
void enlil3DFile::setName(const char *name)
{
    this->_name = QString(name);
}

/**
 * @brief enlil3DFile::addUnitConversion
 * @param baseUnits
 * @param newUnits
 * @param divisor
 */
void enlil3DFile::addUnitConversion(const char *baseUnits, const char *newUnits, double divisor)
{
    this->__addConversion(QString(baseUnits), QString(newUnits), divisor);
}

/**
 * @brief enlil3DFile::getFileName
 * @return
 */
QString enlil3DFile::getFileName()
{
    return this->_fileName;
}

/**
 * @brief enlil3DFile::getName
 * @return
 */
QString enlil3DFile::getName()
{
    return this->_name;
}

/**
 * @brief enlil3DFile::getDims
 * @param xyz
 * @return
 */
int enlil3DFile::getDims(const char* dim)
{
    return this->_dims[QString(dim)];
}

/**
 * @brief enlil3DFile::get3Dcount
 * @return
 */
int enlil3DFile::get3Dcount()
{
    return this->_dims["n1"]*this->_dims["n2"]*this->_dims["n3"];
}

/**
 * @brief enlil3DFile::getGridUnits
 * @return
 */
QString enlil3DFile::getGridUnits()
{
    //TODO: Fix Reader to handle grid unit changes.
    QString Units;
    return Units;
}

/**
 * @brief enlil3DFile::setGridSpacingType
 * @param type
 */
void enlil3DFile::setGridSpacingType(int type)
{

    switch(type)
    {
    case ENLIL_GRIDSPACING_CT:
        this->_gridOutput = &_gridPositionsCT;
        break;

    case ENLIL_GRIDSPACING_SP:
        this->_gridOutput = &_gridPositionsSP;
        break;

    default:
        std::cerr << "Undefined GridSpacing Type" << std::endl;
        this->_gridOutput = NULL;
        break;
    }

}

/**
 * @brief enlil3DFile::getVarNames
 * @return
 */
QStringList enlil3DFile::getVarNames()
{

    return this->_varOutput.keys();

}



/**
 * @brief enlil3DFile::getFileAttributeNames
 * @return
 */
QStringList enlil3DFile::getFileAttributeNames()
{

    return this->_fileAttributeData.keys();

}

/**
 * @brief enlil3DFile::hasUnits
 * @param name
 * @return
 */
bool enlil3DFile::hasUnits(const char *name)
{

    if(this->_varOutput[QString(name)]->Units() != "") return true;
    else return false;
}

/**
 * @brief enlil3DFile::hasUnits
 * @param name
 * @return
 */
bool enlil3DFile::hasUnits(QString name)
{

    return hasUnits(name.toAscii().data());
}

/**
 * @brief enlil3DFile::asFloat
 * @param name
 * @return
 */
QVector<float> enlil3DFile::asFloat(const char *name, int block)
{
    QVector<float> returnValue;
    enlil3DVar* currentVar = this->_varOutput[QString(name)];

    if(currentVar) returnValue = currentVar->asFloat();

    return returnValue;
}

/**
 * @brief enlil3DFile::asFloat
 * @param X
 * @param Y
 * @param Z
 * @return
 */
QVector<QVector<float> > enlil3DFile::asFloat(const char *X, const char *Y, const char *Z, int block)
{

}

/**
 * @brief enlil3DFile::getVar
 * @param name
 * @return
 */
QVector<double> enlil3DFile::asDouble(const char *name, int block)
{
    QVector<double> returnValue;
    enlil3DVar* currentVar = this->_varOutput[QString(name)];

    if(currentVar) returnValue = currentVar->asDouble();

    return returnValue;

}

/**
 * @brief enlil3DFile::asDouble
 * @param X
 * @param Y
 * @param Z
 * @return
 */
QVector<QVector<double> > enlil3DFile::asDouble(const char *X, const char *Y, const char *Z, int block)
{
    QVector<QVector<double> > XYZ;

    QVector<double> X1 = this->asDouble(X);
    QVector<double> Y1 = this->asDouble(Y);
    QVector<double> Z1 = this->asDouble(Z);

    int count = X1.count();

    std::cout << "Count Total: " << count << std::endl;

    int loop = 0;
    for(loop = 0; loop < count; loop++)
    {
        QVector<double> entry;
        entry.push_back(X1[loop]);
        entry.push_back(Y1[loop]);
        entry.push_back(Z1[loop]);

        XYZ.push_back(entry);
    }

    return XYZ;
}

/**
 * @brief enlil3DFile::asInt64
 * @param name
 * @return
 */
QVector<qint64> enlil3DFile::asInt64(const char *name, int block)
{
    QVector<qint64> returnValue;
    enlil3DVar* currentVar = this->_varOutput[QString(name)];

    if(currentVar) returnValue = currentVar->asInt64();

    return returnValue;
}

/**
 * @brief enlil3DFile::asInt64
 * @param X
 * @param Y
 * @param Z
 * @return
 */
QVector<QVector<qint64> > enlil3DFile::asInt64(const char *X, const char *Y, const char *Z, int block)
{

}

/**
 * @brief enlil3DFile::getVarUnits
 * @param name
 * @return
 */
QString enlil3DFile::getVarUnits(const char *name)
{
    if(this->_varOutput.contains(QString(name)))
    {
        return this->_varOutput[QString(name)]->Units();
    }
    else
    {
        return QString();
    }

}

/**
 * @brief enlil3DFile::getVarLongName
 * @param name
 * @return
 */
QString enlil3DFile::getVarLongName(const char *name)
{

    if(this->_varOutput.contains(QString(name)))
    {
        return this->_varOutput[QString(name)]->LongName();
    }
    else
    {
        return QString();
    }
}

/**
 * @brief enlil3DFile::getNumberOfVars
 * @return
 */
qint64 enlil3DFile::getNumberOfVars()
{
    return qint64(this->getVarNames().count());
}

/**
 * @brief enlil3DFile::getNumberOfBlocks
 * @param name
 * @return
 */
qint64 enlil3DFile::getNumberOfBlocks(const char *name)
{
    if(this->_varOutput.contains(QString(name)))
    {
        return this->_varOutput[QString(name)]->recordCount();
    }
    else
    {
        std::cerr << "ERROR: Variable " << name << " Not Found." << std::endl;
    }
    return 0;
}

/**
 * @brief enlil3DFile::getFileAttribute
 * @param name
 * @return
 */
enlilAtt* enlil3DFile::getFileAttribute(const char *name)
{

    if(this->_fileAttributeData.contains(name))
    {
        return this->_fileAttributeData[QString(name)];
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief enlil3DFile::getNumberOfFileAttributes
 * @return
 */
int enlil3DFile::getNumberOfFileAttributes()
{
    return this->_fileAttributeData.count();
}

/**
 * @brief enlil3DFile::selectOutput
 * @param version
 */
void enlil3DFile::selectOutput(int version)
{

    std::cerr << __FUNCTION__ << " is not yet implemented" << std::endl;

}

/**
 * @brief enlil3DFile::getMJD
 * @return
 */
double enlil3DFile::getMJD() const
{

    return _MJD;
}


/**
 * @brief enlil3DFile::__loadVariable
 * @param name
 */
void enlil3DFile::__loadVariable(QString name)
{

    enlil3DVar* newVar = new enlil3DVar(this,name);
    this->_varOutput[name] = newVar;

}


/**
 * @brief enlil3DFile::__loadFileAttribute
 * @param name
 */
void enlil3DFile::__loadFileAttribute(QString name)
{
    NcAtt *attribute = this->_file->get_att(name.toAscii().data());
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

    enlilAtt* newAtt = new enlilAtt(attType, name, variant);

    this->_fileAttributeData[name] = newAtt;

    delete attribute;
}

/**
 * @brief enlil3DFile::__getVaribleList
 * @return
 */
QStringList enlil3DFile::__getVaribleList()
{

    qint64 numVars = this->_file->num_vars();
    QStringList values;

    for(int x=0; x<numVars; x++)
    {
        NcVar* var = this->_file->get_var(x);
        values.push_back(QString(var->name()));
    }

    return values;
}

/**
 * @brief enlil3DFile::__getAttributeList
 * @return
 */
QStringList enlil3DFile::__getAttributeList()
{

    qint64 numAtts = this->_file->num_atts();
    QStringList values;

    for(int x=0; x<numAtts; x++)
    {
        NcAtt* att = this->_file->get_att(x);
        values.push_back(QString(att->name()));
        delete att;
    }

    return values;
}


/**
 * @brief enlil3DFile::__processLocation
 */
void enlil3DFile::__processGridLocations()
{
    int loopX=0, loopY=0, loopZ=0;

    //check existance of the position values
    //TODO: Need selective Read routines that DO NOT store the values permanently.
    QStringList vars = this->_varOutput.keys();

    if(vars.contains("X1") && vars.contains("X2") && vars.contains("X3"))
    {
        QVector<double> x1 = this->_varOutput["X1"]->asDouble();
        QVector<double> x2 = this->_varOutput["X2"]->asDouble();
        QVector<double> x3 = this->_varOutput["X3"]->asDouble();

        QVector<QVector<double> > XYZ;
        QVector<QVector<double> > RTP;

        QVector<double> xyz;

        int xlen = this->getDims("n1");
        int ylen = this->getDims("n2");
        int zlen = this->getDims("n3");

        this->_wholeExtents[QString("n1")] = this->_varOutput["X1"]->getExtent("n1");
        this->_wholeExtents[QString("n2")] = this->_varOutput["X2"]->getExtent("n2");
        this->_wholeExtents[QString("n3")] = this->_varOutput["X3"]->getExtent("n3");
        this->_wholeExtents[QString("nblk")] = this->_varOutput["X1"]->getExtent("nblk");



        /**
             *  xyz is generated and populated from _gridSphere2Cart() routine.
             *  rtp is generated in the following loop.
             **/
        for(loopZ = 0; loopZ < zlen; loopZ++)
        {
            for(loopY = 0; loopY < ylen; loopY++)
            {
                for(loopX = 0; loopX < xlen; loopX++)
                {
                    QVector<double> rtp;

                    if(this->_gridScaleFactor != 1)
                    {
                        rtp.push_back(x1[loopX]/this->_gridScaleFactor);
                    }
                    else
                    {
                        rtp.push_back(x1[loopX]);
                    }

                    rtp.push_back(x2[loopY]);
                    rtp.push_back(x3[loopZ]);

                    xyz = this->__gridSphere2Cart(rtp);

                    RTP.push_back(rtp);
                    XYZ.push_back(xyz);

                }
            }
        }

        //save converted results
        this->_gridPositionsCT = XYZ;
        this->_gridPositionsSP = RTP;

        //define the grid output (Default to Cartesian grid)
        this->setGridSpacingType(ENLIL_GRIDSPACING_CT);
    }
    else
    {
        std::cerr << "ERROR: No Data Found..." << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief enlil3DFile::__processSphericalVectors
 */
void enlil3DFile::__processSphericalVectors()
{
    double divisor = 1;
    QString newUnits;

    QStringList vars = this->_varOutput.keys();
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
//    QVector<double> R = this->_varOutput["X1"];
//    QVector<double> T = this->_varOutput["X2"];
//    QVector<double> P = this->_varOutput["X3"];

    //process the remaining data
//    for(int x = 0; x < vectors.count(); x++)
//    {
//        //skip the location variable
//        if(vectors[x] == "X") continue;

//        //get the units for the FIRST part of the vector... so as not to get radians
//        QPair<QString, double> conversion = this->__getConvDivForVar(vectors[x]+"1");
//        divisor = conversion.second;
//        newUnits = conversion.first;

//        //get variables to process
//        QVector<double> DR = this->_variablesRaw[(QString(vectors[x] + "1").toAscii().data())];
//        QVector<double> DT = this->_variablesRaw[(QString(vectors[x] + "2").toAscii().data())];
//        QVector<double> DP = this->_variablesRaw[(QString(vectors[x] + "3").toAscii().data())];

//        QVector<double> DRc;
//        QVector<double> DTc;
//        QVector<double> DPc;

//        QVector<double> X;
//        QVector<double> Y;
//        QVector<double> Z;

//        double* rtp = new double[3];
//        double* rtpOrigin = new double[3];
//        double* xyz = NULL;

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

//        delete [] rtp;
//        delete [] rtpOrigin;

//        //save processed data (XYZ)
//        this->_variablesProcessed[QString(vectors[x]+"_X")] = X;
//        this->_variablesProcessed[QString(vectors[x]+"_Y")] = Y;
//        this->_variablesProcessed[QString(vectors[x]+"_Z")] = Z;

//        this->_varUnitsProcessed[QString(vectors[x]+"_X")] = newUnits;
//        this->_varUnitsProcessed[QString(vectors[x]+"_Y")] = newUnits;
//        this->_varUnitsProcessed[QString(vectors[x]+"_Z")] = newUnits;


//        //save processed data (RTP)
//        this->_variablesProcessed[QString(vectors[x]+"_R")] = DRc;
//        this->_variablesProcessed[QString(vectors[x]+"_T")] = DTc;
//        this->_variablesProcessed[QString(vectors[x]+"_P")] = DPc;

//        this->_varUnitsProcessed[QString(vectors[x]+"_R")] = newUnits;
//        this->_varUnitsProcessed[QString(vectors[x]+"_T")] = newUnits;
//        this->_varUnitsProcessed[QString(vectors[x]+"_P")] = newUnits;

//    }

}

/**
 * @brief enlil3DFile::__processScalars
 */
void enlil3DFile::__processScalars()
{
//    double divisor=1;
//    QString newUnits;
//    QStringList vars = this->_varOutput.keys();
//    QStringList scalars;
//    QString var;

//    //get list of scalars
//    for(int x = 0; x < vars.size(); x++)
//    {
//        var = vars[x];

//        //get the scalar name
//        if(var.endsWith("1") || var.endsWith("2") || var.endsWith("3")) continue;

//        scalars.push_back(var);
//    }

//    //convert as necessary
//    for(int x = 0; x < scalars.count(); x++)
//    {
//        QVector<double> oldValues = this->_variablesRaw[(scalars[x].toAscii().data())];
//        QVector<double> newValues;

//        QPair<QString, double> conversion = this->__getConvDivForVar(scalars[x]);
//        divisor = conversion.second;
//        newUnits = conversion.first;

//        for(int y = 0; y < oldValues.count(); y++)
//        {
//            newValues.push_back(oldValues[y]/divisor);
//        }

//        this->_variablesProcessed[scalars[x]] = newValues;
//        this->_varUnitsProcessed[scalars[x]] = newUnits;
//        this->_longNamesProcessed[scalars[x]] = this->_longNamesRaw[scalars[x]];
//    }

}

/**
 * @brief enlil3DFile::__processTime
 */
void enlil3DFile::__processTime()
{
    if(!this->_varOutput.keys().contains("TIME"))
    {
        std::cerr << "Missing Variable TIME." << std::endl;
        //        exit(1);
        return; //do not process if doesn't have "TIME"
    }

    if(!this->getFileAttributeNames().contains("version"))
    {
        std::cerr << "Missing Versions..." << std::endl;
        exit(1);
        return; //make sure there is a version
    }

    double version = this->getFileAttribute("version")->getValue().toDouble(); //need to know the version of the run

    // The version of the file dictates the variable name for the reference date
    if(version < 2.8)
    {
        this->_runDataString = QString("refdate_mjd");
    }
    else    // variable name changed in version 2.8
    {
        this->_runDataString = QString("rundate_mjd");
    }

    //If the file does not contain the proper version, throw an error.
    if(!this->getFileAttributeNames().contains(this->_runDataString))
    {
        std::cerr << "ERROR: Enlil Version " << version << " Does not contain the proper reference date variable for this reader.  Please submit a bug report." << std::endl;
        return; //do not process
    }

    //Get the reference date
    double refmjd = this->getFileAttribute(this->_runDataString.toAscii().data())->getValue().toDouble(0);

    //TODO: Get time ... this is a variable read, so I need to figure out how to dynamically do this...
    QVector<double> timeVec = this->_varOutput[QString("TIME")]->asDouble();

    double timeMax = this->__getMax(timeVec);
    if(timeMax > 3.15569e12)
    {
        std::cerr << "Time offset for last time step is more than 100,000 years from the reference time... you may want to check your model output... Cannot calculate a Modified Julian Date for " << qPrintable(this->_fileName)<< std::endl;
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

    //this->_variablesProcessed["MJD"] = mjd;
    this->_MJD = mjd[0];
}

/**
 * @brief enlil3DFile::__addConversion
 *      This method adds a conversion factor to the system.
 *
 * The Conversion is applied to all of the arrays with units that match the FROM units.
 *      The data will be updated using the divisor, and the units description will be
 *      updated to the new Units String.
 * @param baseUnits
 *      Units to be converted FROM
 * @param newUnits
 *      Units to be converted TO
 * @param divisor
 *      divisor that completes the conversion
 */
void enlil3DFile::__addConversion(QString baseUnits, QString newUnits, double divisor)
{
    this->_convMap[baseUnits] = qMakePair(newUnits, divisor);
    this->__processSphericalVectors();
    this->__processScalars();
}

/**
 * @brief enlil3DFile::__getConvDivForVar
 * @param var
 * @return
 */
QPair<QString, double> enlil3DFile::__getConvDivForVar(QString var)
{


//    QString base;
//    if(this->_varUnitsRaw.keys().contains(var))
//    {
//        base = this->_varUnitsRaw[var];
//    }

//    if(this->_convMap.keys().contains(base))
//    {
//        //if there is a conversion loaded, use it
//        return _convMap[base];
//    }
//    else
//    {
//        //else use the original
//        return qMakePair(base, 1.0);
//    }

}

/**
 * @brief enlil3DFile::__gridSphere2Cart
 * @param rtp
 * @return
 */
QVector<double> enlil3DFile::__gridSphere2Cart(const QVector<double> rtp)
{
    //calculate
    QVector<double> xyz;
    xyz.push_back(rtp[0] * sin(rtp[1]) * cos(rtp[2]));
    xyz.push_back(rtp[0] * sin(rtp[1]) * sin(rtp[2]));
    xyz.push_back(rtp[0] * cos(rtp[1]));

    return xyz;
}

/**
 * @brief enlil3DFile::__sphere2Cart
 * @param rtp
 * @param rtpOrigin
 * @return
 */
double *enlil3DFile::__sphere2Cart(const double rtp[], const double rtpOrigin[])
{
    double *vector = new double[3];

    vector[0] = (rtp[0] * sin(rtpOrigin[1]) * cos(rtpOrigin[2])) + (rtp[1] * cos(rtpOrigin[1]) * cos(rtpOrigin[2])) + (-1.0*rtp[2] * sin(rtpOrigin[2]));
    vector[1] = (rtp[0] * sin(rtpOrigin[1]) * sin(rtpOrigin[2])) + (rtp[1] * cos(rtpOrigin[1]) * sin(rtpOrigin[2])) + (rtp[2] * cos(rtpOrigin[2]));
    vector[2] = (rtp[0] * cos(rtpOrigin[1])) + (-1.0*rtp[1] * sin(rtpOrigin[1]));

    return vector;
}

/**
 * @brief enlil3DFile::getScale_factor
 * @return
 */
double enlil3DFile::getScale_factor() const
{
    return _gridScaleFactor;
}

/**
 * @brief enlil3DFile::setScale_factor
 * @param scale_factor
 */
void enlil3DFile::setScale_factor(double scale_factor)
{
    //FIXME: Must re-calculate after scale-factor is set.
    _gridScaleFactor = scale_factor;
}

/**
 * @brief enlil3DFile::getGridSpacing
 * @return
 */
QVector<QVector< double > > enlil3DFile::getGridSpacing()
{
    //FIXME: Get a copy of gridOutput, not the original!
    //TODO: Make the _gridOutput data structure out of qvectors for safety?

    if(!this->_gridOutput)
    {
        std::cerr << "ERROR: Grid Output Not Defined... " <<  __FUNCTION__ << " at line " << __LINE__ << std::endl;
        exit(EXIT_FAILURE);
    }
    QVector<QVector<double > > currentGrid = *this->_gridOutput;

    return currentGrid;
}

/**
 * @brief enlil3DFile::__getMax
 * @param vector
 * @return
 */
double enlil3DFile::__getMax(QVector<double> vector)
{
    QVector<double> sortVector = vector;
    qSort(sortVector);
    double max = sortVector.last();

    return max;
}

/**
 * @brief enlil3DFile::__getExtentDimensions
 * @param extent
 * @return
 */
QVector<int> enlil3DFile::__getExtentDimensions(int extent[6])
{
    QVector<int> dims;
    dims.push_back(extent[1] - extent[0]+1);
    dims.push_back(extent[3] - extent[2]+1);
    dims.push_back(extent[5] - extent[4]+1);

    return dims;
}

