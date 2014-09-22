#include "enlilEvoFile.h"
#include <iostream>
#include <QString>
#include <QStringList>

enlilEvoFile::enlilEvoFile(const char *FileName)
{
    this->setFileName(FileName);

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
        QStringList varAtts = this->_getAttListForVar(vars[x]);
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

}


enlilEvoFile::~enlilEvoFile()
{
    this->file->close();
}

void enlilEvoFile::setFileName(const char *FileName)
{
    this->fileName = QString(FileName);
}

void enlilEvoFile::setName(const char *name)
{
    this->name = QString(name);
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

QVector<float> enlilEvoFile::getVar(const char *name)
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
    float* vals = new float(this->stepCount);

    NcVar *variable = this->file->get_var(name.toAscii().data());
    size_t readDims[2] = {1,1};
    readDims[1] = this->stepCount;

    //read the entire variable
    bool status = variable->get(vals, readDims);

    if(!status)
    {
        std::cerr << "Error Reading variable: " << name.toAscii().data() << std::endl;
    }
    else
    {
        QVector<float> qVals;
        for(int x=0; x < this->stepCount; x++)
        {
            qVals.push_back(vals[x]);
        }

        //save the variable
        this->variables[name] = qVals;

    }

    //free the memory
    delete [] vals;
}

void enlilEvoFile::_loadMetaData(QString name)
{
    NcAtt *attribute = this->file->get_att(name.toAscii().data());
    NcType attType = attribute->type();
    QVariant variant;

    switch(attType)
    {
    case ncChar:
        std::cout << "String Value..." << std::endl;
        variant.fromValue(QString(attribute->as_string(0)));
        break;

    case ncInt:
        std::cout << "Int Value..." << std::endl;
        variant.fromValue(attribute->as_int(0));
        break;

    case ncFloat:
        std::cout << "Float Value..."<< std::endl;
        variant.fromValue(attribute->as_float(0));
        break;

    case ncDouble:
        std::cout << "Double Value..."<< std::endl;
        variant.fromValue(attribute->as_double(0));
        break;

    default:
        std::cerr << "Requested conversion from unknown type" << std::endl;
        break;
    }

    std::cout << "saving value..."<< std::flush<<std::endl;
    this->fileMetaData[name] = variant;

    std::cout << "Value is: " << variant.toString().toAscii().data() << std::flush << std::endl;

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