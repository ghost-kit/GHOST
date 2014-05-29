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
#include "pqPythonManager.h"

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


private:
    void constructor();
    Ui::SwFTdockPanel *ui;

    //helpers
    void initializePython();
    void populateActions();

    //data
    QMap<QString, QString> commandMap;
    QString attatchedScript;

    //python interpreter
    pqPythonManager* pythonManager;
    pqPythonDialog* pythonDialog;


private slots:
   void updateModelDirectory(QString modelSourceDir);
   void executeAnalysisCommand(QString name);

};

#endif // SWFTDOCKPANEL_H
