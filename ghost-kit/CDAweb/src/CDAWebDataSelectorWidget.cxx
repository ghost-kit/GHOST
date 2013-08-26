#include "CDAWebDataSelectorWidget.h"
#include "ui_CDAWebDataSelectorWidget.h"
#include "pqTreeWidgetSelectionHelper.h"
#include "pqActiveObjects.h"
#include "pqSelectionManager.h"


CDAWebDataSelectorWidget::CDAWebDataSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDAWebDataSelectorWidget)
{
    ui->setupUi(this);

    //Setup the Tree Widget
    this->TreeWidget = ui->treeWidget;
    this->TreeWidget->setColumnCount(1);

    //tree widget selection helper
    new pqTreeWidgetSelectionHelper(this->TreeWidget);

    //listen to object changes
    pqActiveObjects *activeObjects = &pqActiveObjects::instance();

}

CDAWebDataSelectorWidget::~CDAWebDataSelectorWidget()
{

    delete ui;
}

void CDAWebDataSelectorWidget::addTreeItem(QTreeWidgetItem *item, QTreeWidgetItem *parent)
{
    if(!parent)
    {
        ui->treeWidget->addTopLevelItem(item);
    }
    else
    {
        //...
    }
}


void CDAWebDataSelectorWidget::treeWidgetCustomContextMenuRequested(const QPoint &pos)
{
}

void CDAWebDataSelectorWidget::dataItemChanged(QTreeWidgetItem *item, int column)
{
}

void CDAWebDataSelectorWidget::currentSelectionChanged()
{
}

void CDAWebDataSelectorWidget::currentTreeItemSelectionChanged()
{
}

void CDAWebDataSelectorWidget::itemSelectionChanged()
{
}
