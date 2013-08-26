#ifndef CDAWEBDATASELECTORWIDGET_H
#define CDAWEBDATASELECTORWIDGET_H

#include "pqComponentsModule.h"

#include <QWidget>
#include <QMap>
#include <QPointer>
#include <QTreeWidget>
#include "pqTimer.h"

namespace Ui {
class CDAWebDataSelectorWidget;
}

class PQCOMPONENTS_EXPORT CDAWebDataSelectorWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CDAWebDataSelectorWidget(QWidget *parent = 0);
    ~CDAWebDataSelectorWidget();

    void addTreeItem(QTreeWidgetItem *item, QTreeWidgetItem *parent=NULL);


public slots:

private slots:
    void treeWidgetCustomContextMenuRequested(const QPoint &pos);
    void dataItemChanged(QTreeWidgetItem *item, int column);
    void currentSelectionChanged();
    void currentTreeItemSelectionChanged();
    void itemSelectionChanged();
    
private:
    QTreeWidget *TreeWidget;
    Ui::CDAWebDataSelectorWidget *ui;
};

#endif // CDAWEBDATASELECTORWIDGET_H
