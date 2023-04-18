#include "itemsloader.h"
#include "ui_itemsloader.h"

ItemsLoader::ItemsLoader(LogItem* _root, LogViewer* _logViewer, QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::ItemsLoader)
    ,logViewer(_logViewer)
    ,mainRootItem(_root)
    ,driver(logViewer->getDbDriver())
    ,tableRootItem(new LogItem(QVector<QVariant>{"TableRoot", "", "", ""}))
{
    ui->setupUi(this);
    tableTreeModel = new TreeModel(tableRootItem, this);
    ui->tableTreeView->setModel(tableTreeModel);

    connect(ui->btn_confirm, &QPushButton::clicked, [this, parent](){
        QModelIndexList indexes = ui->tableTreeView->selectionModel()->selectedRows();
        QVector<LogItem*> selectedItems;
        if (!indexes.empty()){
            selectedItems.reserve(indexes.size());
            selectedItems = tableTreeModel->getSelections(indexes);
        }
        tableTreeModel->beginResetMe();
        for(auto* item: selectedItems){
            tableRootItem->moveChildren(item->childNumber(), mainRootItem);
            loadDataToItem(item);
        }
        tableTreeModel->endResetMe();
        emit needToResetModel("DataBase name: " + logViewer->getQJsonObject().value("DBConfObject")["DatabaseName"].toString());
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
    auto itemsInfo = driver->getItemsInfo();
    auto checkSet = mainRootItem->getSetOfAllTabNames()
            .unite(tableRootItem->getSetOfAllTabNames());
    QVector<QVariant> dataIn{};
    for(const auto& item: tables){
        if(checkSet.contains(item)) continue;
        dataIn.clear();
        dataIn.append(item);
        for(const auto& tableInfo : itemsInfo.value(item))
            dataIn.append(tableInfo);
        auto* newItem = new LogItem(dataIn, tableRootItem);
        tableRootItem->appendChild(newItem);
    }
}

void ItemsLoader::loadDataToItem(LogItem* item){
    auto& itemData = item->getGraphData();
    auto keyColumn = logViewer->getQJsonObject().value("DBConfObject")["KeyColumnName"].toString();
    auto valueColumn = logViewer->getQJsonObject().value("DBConfObject")["ValueColumnName"].toString();
    QVector<QVariant>keysVector;
    QVector<QVariant>valueVector;
    driver->getLogItemData(item->getTableName(), keysVector, keyColumn);
    driver->getLogItemData(item->getTableName(), valueVector, valueColumn);
    if(keysVector.size() != valueVector.size()){
        auto resBtn = QMessageBox::question( this, "Problem while loading DataBase Data",
                                             tr("Key data size not equal value data size\n"),
                                             QMessageBox::Ok);
        return;
    }
    itemData.clear();
    itemData.resize(keysVector.size());
    for (int i = 0; i < keysVector.size(); ++i)
    {
        itemData[i].key = LogViewerItems::prepareKeyData(keysVector[i],logViewer->getKeyType());
        itemData[i].value = valueVector[i].toDouble();
    }
}