#ifndef SWFTDOCKPANEL_H
#define SWFTDOCKPANEL_H

#include <QWidget>
#include <QDockWidget>

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

private:
     void constructor();
    Ui::SwFTdockPanel *ui;
};

#endif // SWFTDOCKPANEL_H
