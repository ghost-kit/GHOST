#ifndef SWFTDOCKPANEL_H
#define SWFTDOCKPANEL_H

#include <QWidget>
#include <QDockWidget>
#include <QString>
#include <QMap>
#include <QList>
#include <QStringList>
#include "gkSwftControlWidget.h"

namespace Ui {
class SwFTdockPanel;
}

class SwFTdockPanel : public QDockWidget
{
    Q_OBJECT
    typedef QDockWidget Superclass;

public:
     SwFTdockPanel(const QString &t, QWidget* p = 0, Qt::WindowFlags f=0): Superclass(t, p, f)
     { this->constructor(); }

     SwFTdockPanel(QWidget *p=0, Qt::WindowFlags f=0): Superclass(p, f)
     { this->constructor(); }

    ~SwFTdockPanel();

     //Function Control
     void addAnalysisFunction(QString name, QString command);
     void removeAnalysisFunction(QString name);
     void attatchMasterScript(QString script);


private:
    void constructor();
    Ui::SwFTdockPanel *ui;

    //helpers
    void initializePython();

    //data
    QMap<QString, QString> commandMap;

private slots:
   void updateModelDirectory(QString modelSourceDir);
   void executeAnalysisCommand(QString name);

};

#endif // SWFTDOCKPANEL_H
