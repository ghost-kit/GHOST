#ifndef ENLILCONTROLFILE_H
#define ENLILCONTROLFILE_H

#include "qstring.h"
#include "qstringlist.h"
#include "qmap.h"
#include "qvector.h"
#include "Qt"
#include "ltrDateTime.h"

#include <fstream>
#include <iostream>



class enlilControlFile
{
public:
    enlilControlFile(const char* FileName);

    void clear();
    void open(const char* FileName);

    //FileName
    QString getFileName();

    //global parameters
    QString getCfProject() const;
    QString getCfDataDir() const;
    QString getCfInData() const;
    int getCarringtonRotation() const;
    int getGrid(int index) const;
    double getX1l() const;
    double getX1r() const;
    double getX2l() const;
    double getX2r() const;
    double getVuTime() const;
    double getTstart() const;
    double getTstop() const;
    double getTtFrom() const;
    double getTtTo() const;
    double getTtStep() const;

    //Fast Solar Wind
    double getDFast() const;
    double getTFast() const;
    double getVFast() const;

    //CME parameters
    int getNCME() const;
    QString getStartDate () const;
    DateTime getCmeDateCloud(int index) const;
    double getCmeLatCloud(int index) const;
    double getCmeLonCloud(int index) const;
    double getCmeRCloud(int index) const;
    double getCmeVelCloud(int index) const;
    double getCmeDCloud(int index) const;
    double getCmeTCloud(int index) const;
    double getCmeXCloud(int index) const;
    double getCmeNCloud(int index) const;
    double getCmeRadCloud(int index) const;

protected:
    QString FileName;

    //global parameters
    QString cfProject;
    int carringtonRotation;
    int grid[3];
    double x1l;
    double x1r;
    double x2l;
    double x2r;
    QString cfDataDir;
    QString cfInData;
    double vuTime;
    double tStart;
    double tStop;
    double ttFrom;
    double ttTo;
    double ttStep;

    //Fast Solar Wind Conditions
    double dFast;
    double tFast;
    double vFast;

    //CME parameters
    int nCME;
    QString startDate;
    QVector<DateTime> cmeDates;
    QVector<double> latCloud;
    QVector<double> lonCloud;
    QVector<double> rCloud;
    QVector<double> velCloud;
    QVector<double> dCloud;
    QVector<double> tCloud;
    QVector<double> xCloud;
    QVector<double> nCloud;
    QVector<double> radCav;


private:
    void zeroAll();
    void processControlFile(const char* FileName);

};

#endif // ENLILCONTROLFILE_H
