#include "CSVItemsLoader.h"

#include <memory>
#include "ui_CSVItemsLoader.h"

CSVItemsLoader::CSVItemsLoader(LogItem* root, LogViewer* incLogViewer, QWidget *parent) :
    QWidget(parent),
    logViewer(incLogViewer),
    mainRootItem(root),
    csvRootItem(new LogItem(QVector<QVariant>{"CSVRoot"})),
    ui(new Ui::CSVItemsLoader)
{
    ui->setupUi(this);
    csvTreeModel = new TreeModel(csvRootItem, this);
    ui->csvTreeView->setModel(csvTreeModel);

    connect(ui->openFile, &QPushButton::clicked, [this](){
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Open csv"), "/home", tr("csv Files (*.csv)"));
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        csvParser = std::make_unique<CSVParser>(fileName);
        reFresh();
        loadDataToItems();
        reFreshView();
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
        emit needToResetModel();
        parent->close();
    });

    connect(ui->csvTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection &selected){
        if(ui->btn_confirm_key->isVisible())
            ui->btn_confirm_key->setEnabled(!selected.empty());
    });

    connect(ui->btn_confirm_key, &QPushButton::clicked, [this](){
        QModelIndexList indexes = ui->csvTreeView->selectionModel()->selectedIndexes();
        if(!indexes.empty())
            setKeyAxisItem(csvTreeModel->getSelections(indexes).first());
        ui->csvTreeView->setSelectionMode(QHeaderView::MultiSelection);
        ui->btn_confirm->setVisible(true);
        ui->btn_confirm_key->setVisible(false);
        ui->btn_confirm->setEnabled(true);
        ui->openFile->setVisible(true);
        ui->mainLabel->setText("CSV Items");
        ui->infoLabel->setText("Choose Items to log");
    });
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
    ui->infoLabel->setText("Choose item to be on X Axis");
    ui->openFile->setVisible(false);
    ui->btn_confirm->setVisible(false);
    ui->btn_confirm_key->setVisible(true);
    ui->btn_confirm_key->setEnabled(false);
    ui->csvTreeView->setSelectionMode(QHeaderView::SingleSelection);
}

void CSVItemsLoader::loadNamesFromCSV(){
    mainRootItem->removeAllChildren();
    auto tables = csvParser->getColumns();
    auto checkSet = mainRootItem->getSetOfAllTabNames()
            .unite(csvRootItem->getSetOfAllTabNames());
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
        for(auto* item : csvRootItem->getMChildItems()){
            int itemDataIdx = csvParser->getColumnIndex(item->getTableName());
            if(itemDataIdx < batch.size())
                item->getGraphData().append(QCPGraphData(batch[keyColumnIndex.value_or(0)].toDouble(), batch[itemDataIdx].toDouble()));
        }
        batch = csvParser->makeNextBatchOfData();
    }
}
