#include <QSqlQuery>
#include "itemsloader.h"
#include "ui_itemsloader.h"

#define MAX_ROWS_IN_SELECT 50000

ItemsLoader::ItemsLoader(LogItem* _root, LogViewer* _logViewer, QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::ItemsLoader)
    ,logViewer(_logViewer)
    ,dateConfDlg(new DateConfDlg(this))
    ,mainRootItem(_root)
    ,driver(std::move(_logViewer->getDbDriver()))
    ,tableRootItem(new LogItem(QVector<QVariant>{"TableRoot", "Date From", "Date To", "Count of Rows"}))
{
    ui->setupUi(this);
    tableTreeModel = new TreeModel(tableRootItem, this);
    ui->tableTreeView->setModel(tableTreeModel);
    ui->progressBar->setVisible(false);
    ui->label->setVisible(false);
    ui->itemsCount->setVisible(false);

    startDate = "1900-01-01";
    endDate = "2099-12-12";
    rowCount = "10000";

    connect(ui->btn_confirm, &QPushButton::clicked, [this](){
        dateConfDlg->loadRefs(startDate, endDate, rowCount);
        dateConfDlg->exec();
    });

    connect(dateConfDlg, &DateConfDlg::dateConfirmed, [this] (const QString& sDate, const QString& eDate, const QString& rCnt){
        startDate = sDate;
        endDate = eDate;
        rowCount = rCnt;

        Thread.quit();
        this->moveToThread(&Thread);
        connect(&Thread, &QThread::started, this, &ItemsLoader::threadWork);
        Thread.start();
    });
}

void ItemsLoader::threadWork(){
    ui->progressBar->setVisible(true);
    ui->label->setVisible(true);
    ui->itemsCount->setVisible(true);

    auto thisDriver = QSharedPointer<PSQL_Driver>(driver);
    thisDriver->setConnection("112233");
    thisDriver->loadAllItemsInfo();

    QModelIndexList indexes = ui->tableTreeView->selectionModel()->selectedRows();
    QVector<LogItem*> selectedItems{tableTreeModel->getSelections(indexes)};

    tableTreeModel->beginResetMe();
    int count = 0;
    for(auto* item: selectedItems){
        loadDataToItem(item, startDate, endDate, rowCount, thisDriver);
        tableRootItem->moveChildren(item->childNumber(), mainRootItem);
        ui->itemsCount->setText(QString("%1").arg(++count));
    }
    tableTreeModel->endResetMe();

    ui->progressBar->setVisible(false);
    ui->label->setVisible(false);
    ui->itemsCount->setVisible(false);
    emit needToResetModel("DataBase name: " + logViewer->getQJsonObject().value("DBConfObject")["DatabaseName"].toString());
}

ItemsLoader::~ItemsLoader()
{
    Thread.quit();
    Thread.wait();
    driver->closeConnection();
    delete ui;
}

void ItemsLoader::reFresh(){
    tableRootItem->removeAllChildren();
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
    dataIn.reserve(10);
    for(const auto& item: tables){
        if(checkSet.contains(item)) continue;
        dataIn.clear();
        dataIn.append(item);
        auto& infoVector = itemsInfo.value(item);
        for(const auto& tableInfo : infoVector)
            dataIn.append(tableInfo);
        auto* newItem = new LogItem(dataIn, tableRootItem);
        tableRootItem->appendChild(newItem);
    }
}

void ItemsLoader::loadDataToItem(LogItem *item, const QString &dateFrom, const QString &dateTo, const QString &count,
                                 const QSharedPointer<PSQL_Driver>& thisDriver){
    auto& itemData = item->getGraphData();
    auto keyColumn = logViewer->getQJsonObject().value("DBConfObject")["KeyColumnName"].toString();
    auto valueColumn = logViewer->getQJsonObject().value("DBConfObject")["ValueColumnName"].toString();
    int rowsIntTable = thisDriver->countOfRowsInTable(item->getTableName());

    ui->progressBar->reset();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(rowsIntTable / MAX_ROWS_IN_SELECT);

    int rowsLoaded;
    int batchCount = 0;

    itemData.clear();
    itemData.reserve(rowsIntTable+1);

    int inLoopFrom, inLoopTo;
    QSqlQuery query;
    while(true){
        inLoopFrom = MAX_ROWS_IN_SELECT * batchCount;
        inLoopTo = (MAX_ROWS_IN_SELECT * (batchCount + 1)) - 1;
        thisDriver->getLogItemData(item->getTableName(), keyColumn, valueColumn, query, dateFrom, dateTo, inLoopFrom, inLoopTo);
        ui->progressBar->setValue(++batchCount);
        rowsLoaded = query.size();
        if(rowsLoaded<1) break;
        while (query.next()){
            itemData.append(
                QCPGraphData(
                    LogViewerItems::prepareKeyData(query.value(0),logViewer->getKeyType()),
                    query.value(1).toDouble()));
        }
    }
}