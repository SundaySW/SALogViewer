#include "itemsloader.h"
#include "ui_itemsloader.h"

ItemsLoader::ItemsLoader(LogItem* _root, LogViewer* _logViewer, QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::ItemsLoader)
    ,logViewer(_logViewer)
    ,mainRootItem(_root)
    ,driver(logViewer->getDbDriver())
    ,tableRootItem(new LogItem(QVector<QVariant>{"TableRoot"}))
{
    ui->setupUi(this);
    tableTreeModel = new TreeModel(tableRootItem, this);
    ui->tableTreeView->setModel(tableTreeModel);

    connect(ui->btn_confirm, &QPushButton::clicked, [this, parent](){
        QModelIndexList indexes = ui->tableTreeView->selectionModel()->selectedIndexes();
        QVector<LogItem*> selectedItems;
        if (!indexes.empty()){
            selectedItems.reserve(indexes.size());
            selectedItems = tableTreeModel->getSelections(indexes);
        }
        tableTreeModel->beginResetMe();
        for(auto* item: selectedItems){
            tableRootItem->moveChildren(item->childNumber(), mainRootItem);
            logViewer->loadItemFromDB(item);
        }
        tableTreeModel->endResetMe();
        emit needToResetModel();
        parent->close();
    });
}

ItemsLoader::~ItemsLoader()
{
    delete ui;
}

void ItemsLoader::reFresh(){
    tableTreeModel->beginResetMe();
    loadNamesFromDB();
    tableTreeModel->endResetMe();
}

void ItemsLoader::loadNamesFromDB() {
    auto tables = driver->getTableNames();
    auto checkSet = mainRootItem->getSetOfAllTabNames()
            .unite(tableRootItem->getSetOfAllTabNames());
    QVector<QVariant> dataIn = {""};
    for(const auto& item: tables){
        if(checkSet.contains(item)) continue;
        dataIn[0] = item;
        auto* newItem = new LogItem(dataIn, tableRootItem);
        tableRootItem->appendChild(newItem);
    }
}