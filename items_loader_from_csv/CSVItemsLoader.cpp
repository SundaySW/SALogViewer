#include "CSVItemsLoader.h"

#include <memory>
#include "ui_CSVItemsLoader.h"
#include "QComboBox"

CSVItemsLoader::CSVItemsLoader(LogItem *root, LogViewer *incLogViewer, LogItem *storedRoot, QWidget *parent) :
    QWidget(parent),
    logViewer(incLogViewer),
    mainRootItem(root),
    csvRootItem(storedRoot),
    ui(new Ui::CSVItemsLoader)
{
    ui->setupUi(this);
    csvTreeModel = new TreeModel(csvRootItem, this);
    ui->XColumnType->setVisible(false);
    ui->csvTreeView->setModel(csvTreeModel);
    ui->btn_confirm->setVisible(false);
    ui->btn_confirm_key->setVisible(false);

    connect(ui->openFile, &QPushButton::clicked, [this](){
        fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open csv"), savedDir_, tr("csv Files (*.csv)"));
        savedDir_ = fileName.remove("*.csv");
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        csvParser = std::make_unique<CSVParser>(fileName);
        reFreshView();
        csvRootItem->removeAllChildren();
        mainRootItem->removeAllChildren();
        reFresh();
        updateItems();
        ui->XColumnType->setVisible(true);
        ui->XColumnType->setCurrentIndex(logViewer->getKeyType());
    });

    connect(ui->btn_confirm, &QPushButton::clicked, [this, parent](){
        QModelIndexList indexes = ui->csvTreeView->selectionModel()->selectedIndexes();
        QVector<LogItem*> selectedItems;
        if (!indexes.empty()){
            selectedItems.reserve(indexes.size());
            selectedItems = csvTreeModel->getSelections(indexes);
        }
        csvTreeModel->beginResetMe();
        for(auto* item: selectedItems)
            csvRootItem->moveChildren(item->childNumber(), mainRootItem);
        csvTreeModel->endResetMe();
        emit needToResetModel("FileName: " + fileName.split('/').last());
        parent->close();
    });

    connect(ui->csvTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            [this](const QItemSelection &selected){
        if(ui->btn_confirm_key->isVisible())
            ui->btn_confirm_key->setEnabled(!selected.empty());
    });

    connect(ui->btn_confirm_key, &QPushButton::clicked, [this](){
        QModelIndexList indexes = ui->csvTreeView->selectionModel()->selectedIndexes();
        if(!indexes.empty())
            setKeyAxisItem(csvTreeModel->getSelections(indexes).first());
        loadDataToItems();
        ui->csvTreeView->setSelectionMode(QHeaderView::MultiSelection);
        ui->btn_confirm->setVisible(true);
        ui->btn_confirm_key->setVisible(false);
        ui->XColumnType->setVisible(false);
        ui->btn_confirm->setEnabled(true);
        ui->openFile->setVisible(true);
        ui->mainLabel->setText("CSV Items");
        ui->infoLabel->setText("Choose Items to log");
    });
    connect(ui->XColumnType,SIGNAL(currentIndexChanged(int)),
            this,SLOT(keyTypeChanged(int)));
}

void CSVItemsLoader::setKeyAxisItem(LogItem* item){
    keyAxisName = item->getTableName();
    keyColumnIndex = csvParser->getColumnIndex(keyAxisName);
    csvTreeModel->beginResetMe();
    csvRootItem->removeChildren(item->childNumber());
    csvTreeModel->endResetMe();
}

void CSVItemsLoader::reFresh(){
    if(csvParser){
        csvTreeModel->beginResetMe();
        loadNamesFromCSV();
        csvTreeModel->endResetMe();
    }
}

void CSVItemsLoader::reFreshView(){
    ui->mainLabel->setText("Key Axis Value");
    ui->infoLabel->setText("Choose type of X Axis data \n and choose item for X Axis");
    ui->openFile->setVisible(false);
    ui->btn_confirm->setVisible(false);
    ui->btn_confirm_key->setVisible(true);
    ui->btn_confirm_key->setEnabled(false);
    ui->csvTreeView->setSelectionMode(QHeaderView::SingleSelection);
}

void CSVItemsLoader::loadNamesFromCSV(){
    auto tables = csvParser->getColumns();
    auto checkSet = mainRootItem->getSetOfAllTabNames()
            .unite(csvRootItem->getSetOfAllTabNames());
    checkSet.insert(keyAxisName);
    QVector<QVariant> dataIn = {""};
    for(const auto& item: tables){
        if(checkSet.contains(item)) continue;
        dataIn[0] = item;
        auto* newItem = new LogItem(dataIn, csvRootItem);
        csvRootItem->appendChild(newItem);
    }
}

CSVItemsLoader::~CSVItemsLoader()
{
    delete ui;
}

void CSVItemsLoader::loadDataToItems() {
    auto batch = csvParser->makeNextBatchOfData();
    while(!batch.empty()){
        if(batch[0] == "update"){
            //updateItems();
        }
        else{
            for(auto* item : csvRootItem->getMChildItems()){
                int itemDataIdx = csvParser->getColumnIndex(item->getTableName());
                if(itemDataIdx>=0 && itemDataIdx < batch.size())
                    item->getGraphData().append(
                            QCPGraphData(
                                    LogViewerItems::prepareKeyData(batch[keyColumnIndex.value_or(0)],
                                                                   logViewer->getKeyType()),
                                    batch[itemDataIdx].toDouble())
                                    );
            }
        }
        batch = csvParser->makeNextBatchOfData();
    }
}

void CSVItemsLoader::updateItems() {
    csvTreeModel->beginResetMe();
    auto tables = csvParser->getColumns();
    auto checkSet = csvRootItem->getSetOfAllTabNames();
    QVector<QVariant> dataIn{};
    dataIn.reserve(2);
    for (const auto &item: tables) {
        if (checkSet.contains(item)) continue;
        dataIn.clear();
        dataIn.append(item);
        auto *newItem = new LogItem(dataIn, csvRootItem);
        csvRootItem->appendChild(newItem);
    }
    csvTreeModel->endResetMe();
}

void CSVItemsLoader::keyTypeChanged(int idx){
    logViewer->setKeyAxisDataType(static_cast<LogViewerItems::KeyType>(idx));
}
