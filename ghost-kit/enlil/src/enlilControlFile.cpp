//This class supports Enlil Control Files for enlil v. 2.7


#include "enlilControlFile.h"
#include "vtkType.h"
#include "qvariant.h"


//Constructor... pretty much everything happens here...
void enlilControlFile::processControlFile(const char* FileName)
{

    this->FileName = QString(FileName);

    //Open File
    std::ifstream inFile(FileName);
    char line[1024];

    while(!inFile.eof())
    {
        inFile.getline(line, 1024);
        QString fileLine(line);

        //remove whitespace from ends
        fileLine = fileLine.trimmed();

        //skip to next line if line is a comment or is blank
        if(fileLine.startsWith('#') || fileLine == "")  continue;

        //if not a comment, split on comment
        fileLine = fileLine.split("#")[0];

        //Split on =, then value on ","
        QStringList varDecl = fileLine.split("=");
        if(varDecl.size() > 1)
        {
            bool isNumber=false;
            QStringList value = varDecl[1].split(",");
            QString key = varDecl[0].trimmed();
            double dvalue = value[0].toDouble(&isNumber);

            //if we have a number, store list as doubles
            if(isNumber)
            {
                for(int n1=0; n1 < value.count(); n1++)
                {
                    QVariant newValue(value[n1].toDouble());
                    controlFile[key].push_back(newValue);
                }
            }
            //if we do not have a number, store list as strings
            else
            {
                for(int n1=0; n1 < value.count(); n1++)
                {
                    controlFile[key].push_back(value[n1]);
                }

            }

        }
        else
        {
            std::cerr << "Failure Reading Control File " << this->FileName.toStdString() << std::endl;
        }
    }

    //close the file
    inFile.close();

}

enlilControlFile::enlilControlFile(const char* FileName)
{
    //initialize the control file
    this->open(FileName);

}

void enlilControlFile::clear()
{
    this->zeroAll();
}

void enlilControlFile::open(const char *FileName)
{
    this->clear();
    this->processControlFile(FileName);
}

bool enlilControlFile::controlFileHasProperty(QString property)
{
    if(this->controlFile.contains(property)) return true;
    else return false;
}

long enlilControlFile::getNumberOfProperties()
{
    return this->controlFile.count();
}

QList<QVariant> enlilControlFile::getProperty(QString name)
{
    if(this->controlFile.contains(name))
        return this->controlFile[name];
}

QList<QVariant> enlilControlFile::getProperty(int index)
{
    QStringList keys = this->controlFile.keys();
    if(this->controlFile.count() > index)
        this->getProperty(keys[index]);
    else
        return QList<QVariant>();
}

QStringList enlilControlFile::getPropertyList()
{
    return this->controlFile.keys();
}

int enlilControlFile::getType(QString name)
{
    bool isNumber=false;
    double x = this->controlFile[name][0].toString().toDouble(&isNumber);

    if(isNumber) return VTK_DOUBLE;

    else return VTK_STRING;
}

void enlilControlFile::zeroAll()
{
    this->controlFile.clear();
}










