#ifndef SWFTDOCKPANEL_H
#define SWFTDOCKPANEL_H

#include <QWidget>
#include <QDockWidget>
#include <QString>
#include <QMap>
#include <QList>
#include <QGridLayout>
#include <QStringList>
#include "gkSwftControlWidget.h"
#include "vtkPythonInterpreter.h"
#include "enlilControlFile.h"
#include "vtksys/ios/iostream"

namespace Ui {
class SwFTdockPanel;
}

class SwFTdockPanel : public QDockWidget
{
    Q_OBJECT
    typedef QDockWidget Superclass;

public:
     SwFTdockPanel(const QString &t, QWidget* p = 0, Qt::WindowFlags f=0);

     SwFTdockPanel(QWidget *p=0, Qt::WindowFlags f=0);

    ~SwFTdockPanel();

     //Function Control
     void addAnalysisFunction(QString name, QString command, QLayout *layout);
     void removeAnalysisFunction(QString name);
     void attatchMasterScript(QString script);


     void hideHeliospherParms();
     void showHeliospherParms();

     bool initSwFT();
signals:
     void processScriptRun(bool state);

private:
    void constructor();
    Ui::SwFTdockPanel *ui;

    //helpers
    void populateActions();
    int findControlFile();

    //data
    QMap<QString, QString> commandMap;
    QString attatchedScript;
    QStringList fileList;
    bool processed;

    //control file info
    QString currentDirectory;
    QString controlFileName;
    enlilControlFile *controlFile;

    //python interpreter
    vtkPythonInterpreter* pythonInterp;




private slots:
   void updateModelDirectory(QString modelSourceDir);
   void processModel();
   void executeAnalysisCommand(QString name);
   void processScriptDone(bool state);

};

#endif // SWFTDOCKPANEL_H
