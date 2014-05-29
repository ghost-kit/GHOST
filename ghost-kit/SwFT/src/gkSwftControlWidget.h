#ifndef GKSWFTCONTROLWIDGET_H
#define GKSWFTCONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class gkSwftControlWidget;
}

class gkSwftControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit gkSwftControlWidget(QWidget *parent = 0);
    ~gkSwftControlWidget();

private:
    Ui::gkSwftControlWidget *ui;
};

#endif // GKSWFTCONTROLWIDGET_H
