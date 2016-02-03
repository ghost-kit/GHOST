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
    if(this->_convMap) delete this->_convMap;
    this->_convMap = new enlilConversion;

    this->__initializeFiles();

    //process extents
    this->__processExtents();

    //process Time
    this->__processTime();
}

enlil3DFile::enlil3DFile(QString FileName, const char *scaleUnits, double scaleFactor)
{
    //setup the object
    this->setFileName(FileName);
    this->_convertData = false;
    this->_useSubExtents = false;
    this->_gridOutput = NULL;
    this->_convMap = NULL;
    this->_scaleFactor = NULL;
    //set scale factor
    this->setScale_factor(QString(scaleUnits), scaleFactor);

    //load the data from the file
    __ResetFile();


}

/**
 * @brief enlil3DFile::~enlil3DFile
 */
enlil3DFile::~enlil3DFile()
{
    if(this->_convMap) delete this->_convMap;
    if(this->_file) delete this->_file;

    QStringList VarKeys = this->_varOutput.keys();
    for(int x=0; x < VarKeys.count(); x++)
    {
        delete this->_varOutput[VarKeys[x]];
    }

    QStringList AttKeys = this->_fileAttributeData.keys();
    for(int x =0; x < AttKeys.count(); x++)
    {
        delete this->_fileAttributeData[AttKeys[x]];
    }

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
    this->_gridOutput = NULL;
    this->_gridPositionsCT.clear();
    this->_gridPositionsSP.clear();
    this->_fileAttributeData.clear();

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
* @brief enlil3DFile::getCurrentExtents
* @return
*/
QMap<QString, enlilExtent> enlil3DFile::getCurrentExtents()
{
    if(this->_useSubExtents)
    {
        return this->_currentExtents;
    }
    else
    {
        return this->_wholeExtents;
    }
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
void enlil3DFile::setFileName(QString FileName)
{
    this->_fileName = FileName;
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
    if(this->_varOutput.contains("X1"))
    {
        return this->_varOutput["X1"]->units();
    }
    else
    {
        return QString();
    }
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

    if(this->_varOutput[QString(name)]->units() != "") return true;
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
    if(currentVar)
    {
        if(this->_useSubExtents)
        {
            QVector<enlilExtent> subExtents;
            subExtents.push_back(this->_currentExtents["n1"]);
            subExtents.push_back(this->_currentExtents["n2"]);
            subExtents.push_back(this->_currentExtents["n3"]);

            returnValue = currentVar->asFloat(subExtents, block);
        }
        else
        {
            returnValue = currentVar->asFloat(block);
        }
    }
    return returnValue;
}

/**
 * @brief enlil3DFile::asFloat
 * @param X
 * @param Y
 * @param Z
 * @return
 */
QVector<QVector<float> > enlil3DFile::asFloat(const char *X, const char *Y, const char *Z, int block, bool cart)
{
    QVector<QVector<float> > XYZ;
    QVector<float> xyz;

    if(!this->contains(X) && !this->contains(Y) && !this->contains(Z))
    {
        return QVector<QVector<float> > ();
    }

    QVector<float> X1 = this->asFloat(X, block);
    QVector<float> Y1 = this->asFloat(Y, block);
    QVector<float> Z1 = this->asFloat(Z, block);

    int X1count = X1.count();
    int Y1count = Y1.count();
    int Z1count = Z1.count();

    if(X1count != Y1count && X1count != Z1count)
    {
        return QVector<QVector<float> > ();
    }

    int count = X1.count();
    int loop = 0;
    for(loop = 0; loop < count; loop++)
    {
        QVector<float> entry;
        if(cart)
        {
            QVector<float> rtp;

            rtp.push_back(X1[loop]);
            rtp.push_back(Y1[loop]);
            rtp.push_back(Z1[loop]);

            xyz = this->__sphere2Cart(rtp);

            entry.push_back(xyz[0]);
            entry.push_back(xyz[1]);
            entry.push_back(xyz[2]);

        }
        else
        {
            entry.push_back(X1[loop]);
            entry.push_back(Y1[loop]);
            entry.push_back(Z1[loop]);
        }
        XYZ.push_back(entry);
    }

    return XYZ;
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

    if(currentVar)
    {
        if(this->_useSubExtents)
        {
            QVector<enlilExtent> subExtents;
            subExtents.push_back(this->_currentExtents["n1"]);
            subExtents.push_back(this->_currentExtents["n2"]);
            subExtents.push_back(this->_currentExtents["n3"]);

            returnValue = currentVar->asDouble(subExtents, block);
        }
        else
        {
            returnValue = currentVar->asDouble(block);
        }
    }


    return returnValue;

}

/**
 * @brief enlil3DFile::asDouble
 * @param X
 * @param Y
 * @param Z
 * @return
 */
QVector<QVector<double> > enlil3DFile::asDouble(const char *X, const char *Y, const char *Z, int block, bool cart)
{
    QVector<QVector<double> > XYZ;
    QVector<double> xyz;

    if(!this->contains(X) && !this->contains(Y) && !this->contains(Z))
    {
        return QVector<QVector<double> > ();
    }

    QVector<double> X1 = this->asDouble(X, block);
    QVector<double> Y1 = this->asDouble(Y, block);
    QVector<double> Z1 = this->asDouble(Z, block);

    int X1count = X1.count();
    int Y1count = Y1.count();
    int Z1count = Z1.count();

    if(X1count != Y1count && X1count != Z1count)
    {
        return QVector<QVector<double> > ();
    }

    int count = X1.count();
    int loop = 0;
    for(loop = 0; loop < count; loop++)
    {
        QVector<double> entry;
        if(cart)
        {
            QVector<double> rtp;

            rtp.push_back(X1[loop]);
            rtp.push_back(Y1[loop]);
            rtp.push_back(Z1[loop]);

            xyz = this->__sphere2Cart(rtp);

            entry.push_back(xyz[0]);
            entry.push_back(xyz[1]);
            entry.push_back(xyz[2]);

        }
        else
        {
            entry.push_back(X1[loop]);
            entry.push_back(Y1[loop]);
            entry.push_back(Z1[loop]);
        }
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

    if(currentVar)
    {
        if(this->_useSubExtents)
        {
            QVector<enlilExtent> subExtents;
            subExtents.push_back(this->_currentExtents["n1"]);
            subExtents.push_back(this->_currentExtents["n2"]);
            subExtents.push_back(this->_currentExtents["n3"]);

            returnValue = currentVar->asInt64(subExtents, block);
        }
        else
        {
            returnValue = currentVar->asInt64(block);
        }
    }
    return returnValue;
}

/**
 * @brief enlil3DFile::asInt64
 * @param X
 * @param Y
 * @param Z
 * @return
 */
QVector<QVector<qint64> > enlil3DFile::asInt64(const char *X, const char *Y, const char *Z, int block, bool cart)
{
    QVector<QVector<qint64> > XYZ;
    QVector<qint64> xyz;

    if(!this->contains(X) && !this->contains(Y) && !this->contains(Z))
    {
        return QVector<QVector<qint64> > ();
    }

    QVector<qint64> X1 = this->asInt64(X, block);
    QVector<qint64> Y1 = this->asInt64(Y, block);
    QVector<qint64> Z1 = this->asInt64(Z, block);

    int X1count = X1.count();
    int Y1count = Y1.count();
    int Z1count = Z1.count();

    if(X1count != Y1count && X1count != Z1count)
    {
        return QVector<QVector<qint64> > ();
    }

    int count = X1.count();
    int loop = 0;
    for(loop = 0; loop < count; loop++)
    {
        QVector<qint64> entry;
        if(cart)
        {
            QVector<qint64> rtp;

            rtp.push_back(X1[loop]);
            rtp.push_back(Y1[loop]);
            rtp.push_back(Z1[loop]);

            xyz = this->__sphere2Cart(rtp);

            entry.push_back(xyz[0]);
            entry.push_back(xyz[1]);
            entry.push_back(xyz[2]);

        }
        else
        {
            entry.push_back(X1[loop]);
            entry.push_back(Y1[loop]);
            entry.push_back(Z1[loop]);
        }
        XYZ.push_back(entry);
    }

    return XYZ;
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
        return this->_varOutput[QString(name)]->units();
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
        return this->_varOutput[QString(name)]->longName();
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

    //TODO: Determine if this is needed
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

    enlil3DVar* newVar = new enlil3DVar(this,name, this->_convMap, this->_scaleFactor);
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
//TODO: Must adjust these to the current extents
void enlil3DFile::__processExtents()
{
    //check existence of vars before trying to access them
    QStringList vars = this->_varOutput.keys();
    if(vars.contains("X1") && vars.contains("X2") && vars.contains("X3"))
    {
        this->_wholeExtents[QString("n1")] = this->_varOutput["X1"]->getExtent("n1");
        this->_wholeExtents[QString("n2")] = this->_varOutput["X2"]->getExtent("n2");
        this->_wholeExtents[QString("n3")] = this->_varOutput["X3"]->getExtent("n3");
        this->_wholeExtents[QString("nblk")] = this->_varOutput["X1"]->getExtent("nblk");
    }
    else
    {
        std::cerr << "ERROR: NO DATA FOUND: Unable to process file extents" << std::endl;
        std::cerr << "FUNCTION: " << __FUNCTION__ << " FILE: " << __FILE__ << std::endl;
        exit(EXIT_FAILURE);
    }
}

void enlil3DFile::__processGridLocations()
{
    int loopX=0, loopY=0, loopZ=0;

    //check existance of the position values
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

                    rtp.push_back(x1[loopX]);
                    rtp.push_back(x2[loopY]);
                    rtp.push_back(x3[loopZ]);

                    xyz = this->__sphere2Cart(rtp);

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
        std::cerr << "FUNCTION: " << __FUNCTION__ << " FILE: " << __FILE__ << std::endl;

        exit(EXIT_FAILURE);
    }
}


/**
 * @brief enlil3DFile::__getScalarList
 */
QStringList enlil3DFile::getScalarList()
{
    QStringList vars = this->_varOutput.keys();
    QStringList scalars;
    QString var;

    for(int x = 0; x < vars.size(); x++)
    {
        //look for vectors of the format ?1, ?2, ?3
        var = vars[x];
        if(!var.endsWith("1") && !var.endsWith("2") && !var.endsWith("3"))
        {
            scalars.push_back(var);
        }
    }
    scalars.removeDuplicates();

    return scalars;
}

/**
 * @brief enlil3DFile::__getVectorList
 */
QStringList enlil3DFile::getVectorList()
{
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
    vectors.removeOne(QString("X"));

    return vectors;
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
    this->_convMap->operator [](baseUnits) = qMakePair(newUnits, divisor);
}


/**
 * @brief enlil3DFile::__gridSphere2Cart
 * @param rtp
 * @return
 */
QVector<double> enlil3DFile::__sphere2Cart(const QVector<double> rtp)
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
 * @return
 */
QVector<float> enlil3DFile::__sphere2Cart(const QVector<float> rtp)
{
    //calculate
    QVector<float> xyz;
    xyz.push_back(rtp[0] * sin(rtp[1]) * cos(rtp[2]));
    xyz.push_back(rtp[0] * sin(rtp[1]) * sin(rtp[2]));
    xyz.push_back(rtp[0] * cos(rtp[1]));

    return xyz;
}

/**
 * @brief enlil3DFile::__sphere2Cart
 * @param rtp
 * @return
 */
QVector<qint64> enlil3DFile::__sphere2Cart(const QVector<qint64> rtp)
{
    //calculate
    QVector<qint64> xyz;
    xyz.push_back(rtp[0] * sin(rtp[1]) * cos(rtp[2]));
    xyz.push_back(rtp[0] * sin(rtp[1]) * sin(rtp[2]));
    xyz.push_back(rtp[0] * cos(rtp[1]));

    return xyz;
}

/**
 * @brief enlil3DFile::getScale_factor
 * @return
 */
double enlil3DFile::getScale_factor() const
{
    if(!this->_scaleFactor->isEmpty())
    {
        QStringList keys = this->_scaleFactor->keys();
        return this->_scaleFactor[0][keys[0]].second;
    }
    else
    {
        return 1.0;
    }
}

/**
 * @brief enlil3DFile::setScale_factor
 * @param scale_factor
 */
void enlil3DFile::setScale_factor(QString units, double scale_factor)
{
    if(!this->_scaleFactor)
    {
        this->_scaleFactor = new enlilConversion();
    }

    this->_scaleFactor[0]["m"] = qMakePair(units, scale_factor);

}

/**
 * @brief enlil3DFile::contains
 * @param var
 * @return
 */
bool enlil3DFile::contains(const char *var)
{
    if(this->_varOutput.contains(QString(var))) return true;
    else return false;
}

/**
 * @brief enlil3DFile::setUseSubExtents
 * @param truth
 */
void enlil3DFile::setUseSubExtents(bool truth)
{
    this->_useSubExtents = truth;
}

/**
 * @brief enlil3DFile::setSubExtents
 * @param subExtents
 */
void enlil3DFile::setSubExtents(int subExtents[])
{
    this->_currentExtents["n1"] = qMakePair(qint64(subExtents[0]), qint64(subExtents[1]));
    this->_currentExtents["n2"] = qMakePair(qint64(subExtents[2]), qint64(subExtents[3]));
    this->_currentExtents["n3"] = qMakePair(qint64(subExtents[4]), qint64(subExtents[5]));

    this->_useSubExtents = true;
    this->__processGridLocations();
}

/**
 * @brief enlil3DFile::getGridSpacing
 * @return
 */
QVector<QVector< double > > enlil3DFile::getGridSpacing()
{
    if(!this->_gridOutput)
    {
        this->__processGridLocations();
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

