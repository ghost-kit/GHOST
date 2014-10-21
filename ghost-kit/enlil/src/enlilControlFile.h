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
    bool controlFileHasProperty(QString property);
    long getNumberOfProperties();
    QList<QVariant> getProperty(QString name);
    QList<QVariant> getProperty(int index);
    QStringList getPropertyList();
    int getType(QString name);


protected:
    QString FileName;
    QMap<QString, QList<QVariant > > controlFile;


private:
    void zeroAll();
    void processControlFile(const char* FileName);

};

#endif // ENLILCONTROLFILE_H
