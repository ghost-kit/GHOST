#ifndef GKSWFTCONTROLWIDGET_H
#define GKSWFTCONTROLWIDGET_H

#include <QString>
#include <QList>
#include <QStringList>
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

    void setFunctionName(QString name);
    void disableMoreButton();
    void enableMoreButton();
    void hideMoreButton();
    void showMoreButton();

private:
    Ui::gkSwftControlWidget *ui;
};

#endif // GKSWFTCONTROLWIDGET_H
