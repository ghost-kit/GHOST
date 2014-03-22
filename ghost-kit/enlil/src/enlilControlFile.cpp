//This class supports Enlil Control Files for enlil v. 2.7


#include "enlilControlFile.h"

//Constructor... pretty much everything happens here...
enlilControlFile::enlilControlFile(const char* FileName)
{
    QMap<QString, QStringList> controlFile;
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
        if(fileLine.startsWith('#') || fileLine == "")
        {
            continue;
        }

        //if not a comment, split on comment
        fileLine = fileLine.split("#")[0];

        //Split on =, then value on ","
        QStringList varDecl = fileLine.split("=");
        if(varDecl.size() > 1)
        {
            controlFile[varDecl[0].trimmed()] = varDecl[1].split(",");
        }
        else
        {
            std::cerr << "Failure Reading Control File " << this->FileName.toStdString() << std::endl;
        }
    }

    //close the file
    inFile.close();

    //zero out the starting values
    this->zeroAll();

    //use input string to populate the object
    //global components
    if(controlFile.contains("project")) this->cfProject = controlFile["project"][0].trimmed();
    if(controlFile.contains("cr")) this->carringtonRotation = controlFile["cr"][0].toInt();
    if(controlFile.contains("x1l")) this->x1l = controlFile["x1l"][0].toDouble();
    if(controlFile.contains("x1r")) this->x1r = controlFile["x1r"][0].toDouble();
    if(controlFile.contains("x2l")) this->x2l = controlFile["x2l"][0].toDouble();
    if(controlFile.contains("x2r")) this->x2r = controlFile["x2r"][0].toDouble();
    if(controlFile.contains("datadir")) this->cfDataDir = controlFile["datadir"][0].trimmed();
    if(controlFile.contains("data")) this->cfInData = controlFile["data"][0].trimmed();
    if(controlFile.contains("vutime")) this->vuTime = controlFile["vutime"][0].toDouble();
    if(controlFile.contains("tstart")) this->tStart = controlFile["tstart"][0].toDouble();
    if(controlFile.contains("tstop")) this->tStop = controlFile["tstop"][0].toDouble();
    if(controlFile.contains("ttfrom")) this->ttFrom = controlFile["ttfrom"][0].toDouble();
    if(controlFile.contains("ttto")) this->ttTo = controlFile["ttto"][0].toDouble();
    if(controlFile.contains("ttstep")) this->ttStep = controlFile["ttstep"][0].toDouble();

    //grid size
    if(controlFile.contains("grd")) this->grid[0] = controlFile["grd"][0].split("x")[0].toInt();
    if(controlFile.contains("grd")) this->grid[1] = controlFile["grd"][0].split("x")[1].toInt();
    if(controlFile.contains("grd")) this->grid[2] = controlFile["grd"][0].split("x")[2].toInt();

    //Fast Solar Wind Components
    if(controlFile.contains("dfast")) this->dFast = controlFile["dfast"][0].toDouble();
    if(controlFile.contains("tfast")) this->tFast = controlFile["tfast"][0].toDouble();
    if(controlFile.contains("vfast")) this->vFast = controlFile["vfast"][0].toDouble();

    //CME parameters
    if(controlFile.contains("ncmes")) this->nCME = controlFile["ncmes"][0].toInt();
    if(controlFile.contains("start")) this->startDate = controlFile["start"][0].trimmed();

    //vector CME parameters (one index for each CME
    //CME Start Dates
    if(controlFile.contains("dates"))
    {
        int numDates = controlFile["dates"].size();
        for(int x = 0; x < numDates; x++)
        {
            //create a new dateTime object
            DateTime newDate;

            //split date from time
            QStringList DateTimeList = controlFile["dates"][x].split("T");

            //configure newDate
            QStringList DateList = DateTimeList[0].split("-");
            QStringList TimeList = DateTimeList[1].split(":");
            newDate.setDay(DateList[2].toInt());
            newDate.setMonth(DateList[1].toInt());
            newDate.setYear(DateList[0].toInt());
            newDate.setHours(TimeList[0].toInt());
            newDate.setMinutes(TimeList[1].toInt());

            //assign to Array
            this->cmeDates.push_back(newDate);


        }
    }

    if(controlFile.contains("lat"))
    {
        int numLats = controlFile["lat"].size();
        for(int x = 0; x < numLats; x++)
        {
            this->latCloud.push_back(controlFile["lat"][x].toDouble());
        }
    }

    if(controlFile.contains("lon"))
    {
        int numLons = controlFile["lon"].size();
        for(int x = 0; x < numLons; x++)
        {
            this->lonCloud.push_back(controlFile["lon"][x].toDouble());
        }
    }

    if(controlFile.contains("rmajor"))
    {
        int numRmajs = controlFile["rmajor"].size();
        for(int x = 0; x < numRmajs; x++)
        {
            this->rCloud.push_back(controlFile["rmajor"][x].toDouble());
        }
    }


    if(controlFile.contains("vcld"))
    {
        int numCmes = controlFile["vcld"].size();
        for(int x = 0; x < numCmes; x++)
        {
            this->velCloud.push_back(controlFile["vcld"][x].toDouble());
        }
    }


    if(controlFile.contains("dcld"))
    {
        int numCmes = controlFile["dcld"].size();
        for(int x = 0; x < numCmes; x++)
        {
            this->dCloud.push_back(controlFile["dcld"][x].toDouble());
        }
    }

    if(controlFile.contains("tcld"))
    {
        int numCmes = controlFile["tcld"].size();
        for(int x = 0; x < numCmes; x++)
        {
            this->tCloud.push_back(controlFile["tcld"][x].toDouble());
        }
    }

    if(controlFile.contains("xcld"))
    {
        int numCmes = controlFile["xcld"].size();
        for(int x = 0; x < numCmes; x++)
        {
            this->xCloud.push_back(controlFile["xcld"][x].toDouble());
        }
    }


    if(controlFile.contains("ncld"))
    {
        int numCmes = controlFile["ncld"].size();
        for(int x = 0; x < numCmes; x++)
        {
            this->nCloud.push_back(controlFile["ncld"][x].toDouble());
        }
    }

    if(controlFile.contains("radcav"))
    {
        int numCmes = controlFile["radcav"].size();
        for(int x = 0; x < numCmes; x++)
        {
            this->radCav.push_back(controlFile["radcav"][x].toDouble());
        }
    }



}




//Accessors
QString enlilControlFile::getCfProject() const
{
    return this->cfProject;
}

QString enlilControlFile::getCfDataDir() const
{
    return this->cfDataDir;
}

QString enlilControlFile::getCfInData() const
{
    return this->cfInData;
}

int enlilControlFile::getCarringtonRotation() const
{
    return this->carringtonRotation;
}

int enlilControlFile::getGrid(int index) const
{
    return this->grid[index];
}

double enlilControlFile::getX1l() const
{
    return this->x1l;
}

double enlilControlFile::getX1r() const
{
    return this->x1r;
}

double enlilControlFile::getX2l() const
{
    return this->x2l;
}

double enlilControlFile::getX2r() const
{
    return this->x2r;
}

double enlilControlFile::getVuTime() const
{
    return this->vuTime;
}

double enlilControlFile::getTstart() const
{
    return this->tStart;
}

double enlilControlFile::getTstop() const
{
    return this->tStop;
}

double enlilControlFile::getTtFrom() const
{
    return this->ttFrom;
}

double enlilControlFile::getTtTo() const
{
    return this->ttTo;
}

double enlilControlFile::getTtStep() const
{
    return this->ttStep;
}


double enlilControlFile::getDFast() const
{
    return dFast;
}


double enlilControlFile::getTFast() const
{
    return tFast;
}


double enlilControlFile::getVFast() const
{
    return vFast;
}


int enlilControlFile::getNCME() const
{
    return nCME;
}

QString enlilControlFile::getStartDate() const
{
    return this->startDate;
}

DateTime enlilControlFile::getCmeDateCloud(int index) const
{
    return this->cmeDates[index];
}

double enlilControlFile::getCmeLatCloud(int index) const
{
    return this->latCloud[index];
}

double enlilControlFile::getCmeLonCloud(int index) const
{
    return this->lonCloud[index];
}

double enlilControlFile::getCmeRCloud(int index) const
{
    return this->rCloud[index];
}

double enlilControlFile::getCmeVelCloud(int index) const
{
    return this->velCloud[index];
}

double enlilControlFile::getCmeDCloud(int index) const
{
    return this->dCloud[index];
}

double enlilControlFile::getCmeTCloud(int index) const
{
    return this->tCloud[index];
}

double enlilControlFile::getCmeXCloud(int index) const
{
    return this->xCloud[index];
}

double enlilControlFile::getCmeNCloud(int index) const
{
    return this->nCloud[index];
}

double enlilControlFile::getCmeRadCloud(int index) const
{
    return this->radCav[index];
}

void enlilControlFile::zeroAll()
{
    //zero out all starting vars

    this->FileName.clear();
    this->cfProject.clear();
    this->cfDataDir.clear();
    this->cfInData.clear();
    this->startDate.clear();

    this->carringtonRotation = 0;
    this->grid[0] = 0;
    this->grid[1] = 0;
    this->grid[2] = 0;

    this->x1l = 0;
    this->x1r = 0;
    this->x2l = 0;
    this->x2r = 0;

    this->vuTime = 0;
    this->tStart = 0;
    this->tStop = 0;
    this->ttFrom = 0;
    this->ttTo = 0;
    this->ttStep = 0;

    this->dFast = 0;
    this->tFast = 0;
    this->vFast = 0;

    this->nCME = 0;
    this->cmeDates.clear();
    this->latCloud.clear();
    this->lonCloud.clear();
    this->rCloud.clear();
    this->velCloud.clear();
    this->dCloud.clear();
    this->tCloud.clear();
    this->xCloud.clear();
    this->nCloud.clear();
    this->radCav.clear();
}





