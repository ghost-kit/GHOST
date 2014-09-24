#include "enlilEvoFile.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <vtkMath.h>

enlilEvoFile::enlilEvoFile(const char *FileName)
{
    //setup the object
    this->setFileName(FileName);
    this->__scale_factor = 1;


    //load the data from the file
    this->_initializeFiles();

    //process location
    this->_processLocation();

}

void enlilEvoFile::_initializeFiles()
{
    char* FileName = this->fileName.toAscii().data();

    //open the file
    std::cout << "Opening file: " << FileName << std::endl;
    this->file = new NcFile(FileName);

    if(this->file->is_valid())
        std::cout << "File Loaded..." << std::flush << std::endl;
    else
        std::cout << "File Failed to open..." << std::flush << std::endl;

    //get number of entries
    NcDim *dims = this->file->get_dim("nevo");
    std::cout << "Dims retrieved..." << std::endl;
    if(dims->is_valid())
    {
        this->stepCount = dims->size();
        std::cerr << "stepCount: " << this->stepCount << std::endl;
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


        /** TODO::This section causes segfault.... must fix before uncommenting **/
        //        QStringList varAtts = this->_getAttListForVar(vars[x]);
        //        for(int y=0; y < varAtts.size();y++)
        //        {
        //            std::cout << "getting var attributes.." << std::flush << std::endl;
        //            //get specified vars if they exist
        //            if(varAtts[y] == QString("units") || varAtts[y] == QString("long_name"))
        //                this->_loadAttFromVar(vars[x], varAtts[y]);
        //        }

    }

    //process atts
    for(int x=0; x < atts.size(); x++)
    {
        std::cout << "Loading Attribute: " << atts[x].toAscii().data() << std::endl;
        this->_loadMetaData(atts[x]);
    }

    std::cout << "Initialization finished..." << std::endl;

    this->file->close();

}

enlilEvoFile::~enlilEvoFile()
{
    //this->file->close();
}

void enlilEvoFile::setFileName(const char *FileName)
{
    this->fileName = QString(FileName);
}

void enlilEvoFile::setName(const char *name)
{
    this->name = QString(name);
}

void enlilEvoFile::setScaleFactor(double factor)
{
    this->__scale_factor = factor;
}

QString enlilEvoFile::getFileName()
{
    return this->fileName;
}

QString enlilEvoFile::getName()
{
    return this->name;
}

qint64 enlilEvoFile::getStepCount()
{
    return this->stepCount;
}

QStringList enlilEvoFile::getVarNames()
{
    return this->variables.keys();
}

QStringList enlilEvoFile::getMeataDataNames()
{
    return this->fileMetaData.keys();
}

QVector<double> enlilEvoFile::getVar(const char *name)
{
    return this->variables[QString(name)];
}

QString enlilEvoFile::getVarUnits(const char *name)
{
    return this->units[QString(name)];
}

QString enlilEvoFile::getVarLongName(const char *name)
{
    return this->longNames[QString(name)];
}

QVariant enlilEvoFile::getMetaData(const char *name)
{
    return this->fileMetaData[QString(name)];
}

void enlilEvoFile::_loadVariable(QString name)
{

    std::cout << "Loading Variable..." << name.toAscii().data() << std::endl;
    long readStart[2] = {0,0};

    NcVar *variable = this->file->get_var(name.toAscii().data());
    variable->set_cur(readStart);


    //get the values from file
    NcValues* values = variable->values();
    int numElem = values->num();

    std::cout << "numElem: " << numElem << std::endl;

    //create a data structure for storage
    QVector<double> qVals;
    for(int x=0; x < numElem; x++)
    {
        qVals.push_back(values->as_double(x));
        if(x%100 == 0) std::cout << "[" << x << "]: " << values->as_double(x) << std::endl;
    }

    //save the variable
    this->variables[name] = qVals;

    //free the memory
    delete values;
}

void enlilEvoFile::_loadMetaData(QString name)
{
    NcAtt *attribute = this->file->get_att(name.toAscii().data());
    NcType attType = attribute->type();
    QVariant variant;

    switch(attType)
    {
    case ncChar:
    {
        std::cout << "String Value..." << std::endl;
        QVariant value(QString(attribute->as_string(0)));
        variant = value;
        break;
    }

    case ncInt:
    {
        std::cout << "Int Value..." << std::endl;
        QVariant value(attribute->as_int(0));
        variant = value;
        break;
    }

    case ncFloat:
    {
        std::cout << "Float Value..."<< std::endl;
        QVariant value(attribute->as_float(0));
        variant = value;
        break;
    }

    case ncDouble:
    {
        std::cout << "Double Value..."<< std::endl;
        QVariant value(attribute->as_double(0));
        variant = value;
        break;
    }

    default:
        std::cerr << "Requested conversion from unknown type" << std::endl;
        break;
    }

    std::cout << "saving value..."<< std::flush<<std::endl;
    this->fileMetaData[name] = variant;

    std::cout << "Value is: " << variant.toString().toAscii().data() << std::flush << std::endl;

    std::cout << "Returning to calling..." << std::endl;
}

QStringList enlilEvoFile::_getVaribleList()
{
    std::cout << "Getting number of Vars" << std::flush <<std::endl;
    qint64 numVars = this->file->num_vars();
    QStringList values;

    for(int x=0; x<numVars; x++)
    {
        std::cout << "Getting Variable: " << x << std::flush << std::endl;
        NcVar* var = this->file->get_var(x);
        std::cout << "Name: " << var->name() << std::endl;
        values.push_back(QString(var->name()));
    }

    std::cout << "Returning to call function..." << std::flush << std::endl;
    return values;
}

QStringList enlilEvoFile::_getAttributeList()
{
    std::cout << "Getting number of Attributes" << std::flush <<std::endl;
    qint64 numAtts = this->file->num_atts();
    QStringList values;

    for(int x=0; x<numAtts; x++)
    {
        std::cout << "Getting Attribute: " << x << std::endl;
        NcAtt* att = this->file->get_att(x);
        values.push_back(QString(att->name()));
        std::cout << "Name: " << att->name() << std::flush << std::endl;
    }

    std::cout << "Returning to calling function..." << std::endl;
    return values;
}

void enlilEvoFile::_loadAttFromVar(QString VarName, QString AttName)
{
    QString value;
    NcVar* var = this->file->get_var(VarName.toAscii().data());
    NcAtt* att = var->get_att(AttName.toAscii().data());

    value = QString(att->as_string(0));

    if(AttName == "units")
    {
        this->units[VarName] = value;
    }
    else if(AttName == "long_name")
    {
        this->longNames[VarName] = value;
    }
}

QStringList enlilEvoFile::_getAttListForVar(QString varName)
{
    QStringList vals;
    NcVar* var = this->file->get_var(varName.toAscii().data());

    int numAtts = var->num_atts();

    for(int x = 0; x < numAtts; x++)
    {
        NcAtt* att = var->get_att(x);
        vals.push_back(att->name());
    }
}

void enlilEvoFile::_processLocation()
{
    //check existance of the position values
    QStringList vars = this->getVarNames();

    if(vars.contains("X1") && vars.contains("X2") && vars.contains("X3"))
    {
        QVector<double> x1 = this->getVar("X1");
        QVector<double> x2 = this->getVar("X2");
        QVector<double> x3 = this->getVar("X3");

        QVector<double> X;
        QVector<double> Y;
        QVector<double> Z;

        double* rtp = new double[3];
        double* xyz = NULL;

        for(int a = 0; a < this->stepCount; a++)
        {

            rtp[0] = x1[a];
            rtp[1] = x2[a];
            rtp[2] = x3[a];

           xyz = this->_gridSphere2Cart(rtp);

            X.push_back(xyz[0]);
            Y.push_back(xyz[1]);
            Z.push_back(xyz[2]);

            delete [] xyz;

        }

        delete [] rtp;

        this->variables["_posX"] = X;
        this->variables["_posY"] = Y;
        this->variables["_posZ"] = Z;

    }

}

double* enlilEvoFile::_gridSphere2Cart(const double rtp[], double scale)
{
    double sf = 0;

    //fix scale factor
    if(!scale) sf = this->__scale_factor;
    else sf = scale;

    //calculate
    double* xyz = new double[3];
    xyz[0] = rtp[0] * sin(rtp[1]) * cos(rtp[2])/this->__scale_factor;
    xyz[1] = rtp[0] * sin(rtp[1]) * sin(rtp[2])/this->__scale_factor;
    xyz[2] = rtp[0] * cos(rtp[1])/this->__scale_factor;

    return xyz;

}

double *enlilEvoFile::_sphere2Cart(const double rtp[], const double rtpOrigin[])
{
    double *vector = new double[3];

    //TODO: Finish this conversion


    return vector;
}
