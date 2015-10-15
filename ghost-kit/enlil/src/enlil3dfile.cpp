#include "enlil3dfile.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <vtkMath.h>
#include "ltrDateTime.h"

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
    this->_retainRaw = false;
    this->_gridOutput = NULL;
    this->_varOutput = NULL;
    this->_varUnitsOutput = NULL;
    this->_longNamesOutput = NULL;


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
    this->_extents[0]=0;
    this->_extents[1]=0;
    this->_extents[2]=0;
    this->_extents[3]=0;
    this->_extents[4]=0;
    this->_extents[5]=0;

    this->_gridOutput = NULL;
    this->_gridPositionsCT.clear();
    this->_gridPositionsSP.clear();
    this->_gridUnits.clear();

    this->_varOutput = NULL;
    this->_variablesRaw.clear();
    this->_variablesProcessed.clear();

    this->_varUnitsOutput = NULL;
    this->_varUnitsRaw.clear();
    this->_varUnitsProcessed.clear();

    this->_longNamesOutput = NULL;
    this->_longNamesRaw.clear();
    this->_longNamesProcessed.clear();

    this->_fileAttributeData.clear();
    this->_convMap.clear();

}

/**
 * @brief enlil3DFile::cleanVar
 * @param _name
 */
void enlil3DFile::cleanVar(const char *_name)
{
    std::cerr << __FUNCTION__ << " Not Implemented" << std::endl;
    exit(EXIT_FAILURE);
}

/**
 * @brief enlil3DFile::setExtents
 * @param extents
 */
void enlil3DFile::setExtents(int extents[6])
{
    this->__setUseExtents(extents);
}

/**
 * @brief enlil3DFile::setExtents
 * @param X1
 * @param X2
 * @param Y1
 * @param Y2
 * @param Z1
 * @param Z2
 */
void enlil3DFile::setExtents(int X1, int X2, int Y1, int Y2, int Z1, int Z2)
{
    int extents[6] = {X1, X2, Y1, Y2, Z1, Z2};
    this->__setUseExtents(extents);
}

/**
 * @brief enlil3DFile::getExtents
 * @return
 */
QVector<int> enlil3DFile::getExtents()
{
    QVector<int> returnVal;
    returnVal.push_back(this->_extents[0]);
    returnVal.push_back(this->_extents[1]);
    returnVal.push_back(this->_extents[2]);
    returnVal.push_back(this->_extents[3]);
    returnVal.push_back(this->_extents[4]);
    returnVal.push_back(this->_extents[5]);

    return returnVal;
}

/**
 * @brief enlil3DFile::setUseExtents
 * @param truth
 */
void enlil3DFile::__setUseExtents(int extents[6])
{

    //reset the current data
    this->__cleanAll();

    //set the extents needed
    this->_extents[0] = extents[0];
    this->_extents[1] = extents[1];
    this->_extents[2] = extents[2];
    this->_extents[3] = extents[3];
    this->_extents[4] = extents[4];
    this->_extents[5] = extents[5];

    //set the use extents
    this->_useExtents = true;

    //initialize the files
    this->__ResetFile();

}

/**
 * @brief enlil3DFile::__initializeFiles
 */
void enlil3DFile::__initializeFiles()
{
    this->selectOutput(0);

    //open the file
    this->_file = new NcFile(qPrintable(this->_fileName));

    if(!this->_file->is_valid())
    {
        std::cerr << "Error loading file " << qPrintable(this->_fileName) << std::endl;
        return;
    }

    //get number of entries
    NcDim *dims[3];
    dims[0] = this->_file->get_dim("n1");
    dims[1] = this->_file->get_dim("n2");
    dims[2] = this->_file->get_dim("n3");


    if(dims[0]->is_valid() && dims[1]->is_valid() && dims[2]->is_valid())
    {
        this->_dims[0] = dims[0]->size();
        this->_dims[1] = dims[1]->size();
        this->_dims[2] = dims[2]->size();
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

    //process vars
    //TODO: create method for loading the actual data.
    if(vars.contains(QString("TIME")))
    {
        this->__loadVariable(QString("TIME"));
        this->_TIME = this->_variablesRaw["TIME"][0];

    }

    //check to see if we are operating on extents
    if(!this->_useExtents)
    {
        //Not using extents... ==> use entire file
        for(int x=0; x < vars.size(); x++)
        {
            this->__loadVariable(vars[x]);

            QStringList varAtts = this->__getAttListForVar(vars[x]);

            for(int y=0; y < varAtts.size();y++)
            {
                this->__loadAttFromVar(vars[x], varAtts[y]);
            }
        }
    }
    else
    {
        //Using Extents... ==> use selected extents only.

        QVector<int> newDims = this->__getExtentDimensions(this->_extents);
        this->_dims[0] = newDims[0];
        this->_dims[1] = newDims[1];
        this->_dims[2] = newDims[2];

        for(int x=0; x < vars.size();x++)
        {
            this->__loadVariableExtents(vars[x]);
        }



    }


    this->_file->close();
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
int enlil3DFile::getDims(int xyz)
{
    return this->_dims[xyz];
}

/**
 * @brief enlil3DFile::get3Dcount
 * @return
 */
int enlil3DFile::get3Dcount()
{
    return this->_dims[0]*this->_dims[1]*this->_dims[2];
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
    if(!this->_varOutput)
    {
        std::cerr << "Var Output is not defined.  Fix Me!" << std::endl << std::flush;
        exit(EXIT_FAILURE);
    }

    return this->_varOutput->keys();

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

    if(this->_varUnitsOutput->contains(QString(name))) return true;
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
 * @brief enlil3DFile::getVar
 * @param name
 * @return
 */
QVector<double> enlil3DFile::getVar(const char *name)
{

    return this->_varOutput->operator [](QString(name));

}

QVector<QVector<double> > enlil3DFile::getVar(const char *X, const char *Y, const char *Z)
{
    QVector<QVector<double> > XYZ;

    QVector<double> X1 = this->getVar(X);
    QVector<double> Y1 = this->getVar(Y);
    QVector<double> Z1 = this->getVar(Z);

    int loop = 0;
    for(loop = 0; loop < this->get3Dcount(); loop++)
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
 * @brief enlil3DFile::getVarUnits
 * @param name
 * @return
 */
QString enlil3DFile::getVarUnits(const char *name)
{

    return this->_varUnitsOutput->operator [](QString(name));
}

/**
 * @brief enlil3DFile::getVarLongName
 * @param name
 * @return
 */
QString enlil3DFile::getVarLongName(const char *name)
{

    return this->_longNamesOutput->operator [](QString(name));

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
 * @brief enlil3DFile::getFileAttribute
 * @param name
 * @return
 */
QVariant enlil3DFile::getFileAttribute(const char *name)
{

    return this->_fileAttributeData[QString(name)];

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

    switch(version)
    {
    case 0:
        this->_varOutput = &_variablesRaw;
        this->_varUnitsOutput = &_varUnitsRaw;
        this->_longNamesOutput = &_longNamesRaw;
        break;

    case 1:
        this->_varOutput = &_variablesProcessed;
        this->_varUnitsOutput = &_varUnitsProcessed;
        this->_longNamesOutput = &_longNamesProcessed;
        break;

    default:
        break;
    }
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

    long readStart[2] = {0,0};

    NcVar *variable = this->_file->get_var(name.toAscii().data());
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

/**
 * @brief enlil3DFile::__loadVariableExtents
 * @param _name
 */
void enlil3DFile::__loadVariableExtents(QString _name)
{
    //FIXME: This is currently a direct copy from the old reader.  Must modify.
    int extDims[3] = {0,0,0};
    size_t readDims[4]   = {1,1,1,1};
    long readStart[4]  = {0,extents[4],extents[2],extents[0]};

    // get dimensions from extents
    this->extractDimensions(extDims, extents);

    // Enlil encodes in reverse, so reverse the order, add fourth dimension 1st.
    readDims[1] = extDims[2];
    readDims[2] = extDims[1];
    readDims[3] = extDims[0];

    //find all conditions that need to be accounted for
    bool periodic = false;
    bool periodicRead = false;
    bool periodicOnly = false;

    // this->printExtents(extents, (char*)"Loading Extents: ");

    if(extents[5] == this->WholeExtent[5])
    {
        periodic = true;
        // std::cout << "Set Periodic" << std::endl;

        if(extents[4] > 0)
        {
            periodicRead = true;
            // std::cout << "Set Periodic Read" << std::endl;
            if(extents[4] == this->WholeExtent[5])
            {
                periodicOnly = true;
                // std::cout << "Set Periodic Only" << std::endl;

            }
        }
    }
    else
    {
        //  std::cout << "Non-Periodic" << std::endl;
        //  dont need to do anything

    }

    // allocate memory for complete array
    double *array = new double[extDims[0]*extDims[1]*extDims[2]];

    //open file
    NcFile file(this->FileName);
    NcVar* variable = file.get_var(arrayName);

    // start to read in data
    if(periodic && !periodicOnly)
    {
        //adjust dims
        readDims[1] = readDims[1]-1;

        //adjust the start point
        variable->set_cur(readStart);

        //read the file
        variable->get(array, readDims);

    }
    else if(periodicOnly)
    {
        //set periodic only
        readDims[1] = 1;
        readStart[1] = 0;
        readStart[2] = extents[2];
        readStart[3] = extents[0];

        //set read location
        variable->set_cur(readStart);

        //read the file
        variable->get(array, readDims);

    }
    else
    {
        //set read location as stated
        variable->set_cur(readStart);

        //read as stated
        variable->get(array, readDims);

    }

    // fix periodic boundary if necesary
    if(periodic && !periodicRead && !periodicOnly)
    {
        //copy periodic data from begining to end
        size_t wedgeSize = (extDims[0]*extDims[1]);
        size_t wedgeLoc  = (extDims[0]*extDims[1])*(extDims[2]-1);

        for(int x = 0; x < wedgeSize; x++)
        {
            //copy the wedge
            array[wedgeLoc] = array[x];

            //advance index
            wedgeLoc++;
        }

    }
    else if (periodic && periodicRead && !periodicOnly)  /*periodicRead &&*/
    {
        //read in periodic data and place at end of array
        size_t wedgeSize = extDims[0]*extDims[1];
        size_t wedgeLoc  = (extDims[0]*extDims[1])*(extDims[2]-1);

        double * wedge = new double[wedgeSize];

        //start at 0,0,0
        readStart[0] = 0;
        readStart[1] = 0;
        readStart[2] = extents[2];
        readStart[3] = extents[0];

        //restrict to phi = 1 dimension
        readDims[1] = 1;

        //set start
        variable->set_cur(readStart);

        //read data
        variable->get(wedge, readDims);

        //populate wedge to array
        for(int x = 0; x < wedgeSize; x++)
        {
            //copy the wedge
            array[wedgeLoc] = wedge[x];

            //advance index
            wedgeLoc++;
        }

        //free temp memory
        delete [] wedge; wedge = NULL;
    }


    //close file
    file.close();
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

    this->_fileAttributeData[name] = variant;

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
 * @brief enlil3DFile::__loadAttFromVar
 * @param VarName
 * @param AttName
 */
void enlil3DFile::__loadAttFromVar(QString VarName, QString AttName)
{
    //TODO: There should be a better way to handle Varaiable Attributes
    //TODO: Find way to handle any variable attribute.
    QString value;
    NcVar* var = this->_file->get_var(VarName.toAscii().data());
    NcAtt* att = var->get_att(AttName.toAscii().data());

    //FIXME: Memory Leak when doing this.  Need to controll the att->as_string memory apparently.
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

    delete att;
    //    delete var;

}

/**
 * @brief enlil3DFile::__getAttListForVar
 * @param varName
 * @return
 */
QStringList enlil3DFile::__getAttListForVar(QString varName)
{
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

    NcVar* var = this->_file->get_var(qPrintable(varName));
    qint64 numAtts = var->num_atts();

    for(int x=0; x<numAtts; x++)
    {
        NcAtt* att = var->get_att(x);
        if(att->is_valid()) values.push_back(QString(att->name()));
        delete att;
    }

    //delete var;
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
    QStringList vars = this->_variablesRaw.keys();

    if(vars.contains("X1") && vars.contains("X2") && vars.contains("X3"))
    {
        QVector<double> x1 = this->_variablesRaw["X1"];
        QVector<double> x2 = this->_variablesRaw["X2"];
        QVector<double> x3 = this->_variablesRaw["X3"];

        QVector<QVector<double> > XYZ;
        QVector<QVector<double> > RTP;

        QVector<double> xyz;

        int xlen = this->getDims(0);
        int ylen = this->getDims(1);
        int zlen = this->getDims(2);

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
        QPair<QString, double> conversion = this->__getConvDivForVar(vectors[x]+"1");
        divisor = conversion.second;
        newUnits = conversion.first;

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

/**
 * @brief enlil3DFile::__processScalars
 */
void enlil3DFile::__processScalars()
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

        QPair<QString, double> conversion = this->__getConvDivForVar(scalars[x]);
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

/**
 * @brief enlil3DFile::__processTime
 */
void enlil3DFile::__processTime()
{
    if(!this->_variablesRaw.keys().contains("TIME"))
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

    double version = this->getFileAttribute("version").toDouble(); //need to know the version of the run

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
    double refmjd = this->getFileAttribute(this->_runDataString.toAscii().data()).toDouble(0);

    //TODO: Get time ... this is a variable read, so I need to figure out how to dynamically do this...
    QVector<double> timeVec = this->_variablesRaw[("TIME")];

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

    this->_variablesProcessed["MJD"] = mjd;
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
    qSort(vector);
    double max = vector.last();

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

