
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#ifdef _BUILD_TYPE_
#define CURRENT_BUILD_TYPE_ _BUILD_TYPE_
#else
#define CURRENT_BUILD_TYPE_ "CHECK CMAKE"
#endif

QRandomGenerator generator = QRandomGenerator();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , loaderCSVDlg(new QDialog(this))
    , loaderDBDlg(new QDialog(this))
    , rootItem(new LogItem(QVector<QVariant>{"root"}))
    , storedRoot(new LogItem(QVector<QVariant>{"StoredRoot"}))
    , treeModel(new TreeModel(rootItem, this))
{
    configureUI();
    openFileLoadConfig();
    logViewer = new LogViewer(ConfJson, Plot, rootItem, storedRoot, this);
    DBitemsLoader = new ItemsLoader(rootItem, logViewer, loaderDBDlg);
    csvItemsLoader = new CSVItemsLoader(rootItem, logViewer, storedRoot, loaderCSVDlg);
    settingsDlg = new Settings_dlg(logViewer, ConfJson, this);
    configDBLoaderDlg();
    configCSVLoaderDlg();
    setConnections();
}

void MainWindow::configureUI(){
    ui->setupUi(this);
    Plot = ui->Plot;
    ui->treeView->setModel(treeModel);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::configDBLoaderDlg(){
    loaderDBDlg->setModal(true);
    loaderDBDlg->setWindowTitle("Load DB Items");
    auto* diagLayout = new QVBoxLayout();
    diagLayout->addWidget(DBitemsLoader);
    loaderDBDlg->setLayout(diagLayout);
}

void MainWindow::configCSVLoaderDlg(){
    loaderCSVDlg->setModal(true);
    loaderCSVDlg->setWindowTitle("Load CSV Items");
    auto* diagLayout = new QVBoxLayout();
    diagLayout->addWidget(csvItemsLoader);
    loaderCSVDlg->setLayout(diagLayout);
}

void MainWindow::setConnections(){
    ui->openGL_Btn->setChecked(Plot->openGl());
    connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ContextMenuRequested(const QPoint &)));

    connect(treeModel, &TreeModel::ItemMoved, [this](LogItem* who, LogItem* where, LogItem* from){
        if(from->isIamContainer() && !from->childCount()){
            logViewer->removeItem(from);
            treeModel->reFresh();
        }
        if(who->isActive())
            logViewer->rebuildLayout();
    });
    connect(ui->openGL_Btn, &QPushButton::clicked, [this](){
        ui->openGL_Btn->setChecked(logViewer->manageOpenGl());
    });
    connect(ui->settings_btn, &QPushButton::clicked, [this]{
        settingsDlg->show();
        settingsDlg->raise();
        settingsDlg->activateWindow();
    });
    connect(ui->loadDBParams_btn, &QPushButton::clicked, [this](){
        DBitemsLoader->reFresh();
        loaderDBDlg->exec();
    });
    connect(ui->loadCSVParams_btn, &QPushButton::clicked, [this](){
        treeModel->beginResetMe();
        csvItemsLoader->reFresh();
        treeModel->endResetMe();
        loaderCSVDlg->exec();
    });
    connect(DBitemsLoader, &ItemsLoader::needToResetModel, [this](const QString& dbname){
        treeModel->reFresh();
        ui->dbLabel->setText(dbname);
        lastSource = LogViewerItems::DataBase;
    });
    connect(csvItemsLoader, &CSVItemsLoader::needToResetModel, [this](const QString& fileName){
        treeModel->reFresh();
        ui->dbLabel->setText(fileName);
        lastSource = LogViewerItems::CSV;
        logViewer->rebuildLayout();
    });
    connect(logViewer, &LogViewer::errorInDBToLog, [this](const QString& errorStr){
        AddToLog(errorStr,true);
        checkServicesConnection();
    });
    connect(logViewer, &LogViewer::eventInDBToLog, [this](const QString& eventStr){ AddToLog(eventStr);});
    connect(settingsDlg, &Settings_dlg::eventInSettings, [this](const QString& str, bool err){
        AddToLog(str, err);
        checkServicesConnection();
    });

}

void MainWindow::setupModelData(){
    QVector<QVariant> dataIn;
    dataIn << "PWM";
    auto* newItem = new LogItem(dataIn, rootItem);
    rootItem->appendChild(newItem);
    dataIn[0] = "RPM";
    newItem->appendChild(new LogItem(dataIn, newItem));
    dataIn[0] = "IDC";
    rootItem->appendChild(new LogItem(dataIn, rootItem));
    dataIn[0] = "ECT";
    newItem->appendChild(new LogItem(dataIn, newItem));
}

void MainWindow::openFileLoadConfig(){
    auto pathToFile = QString(CURRENT_BUILD_TYPE_) == "Debug" ? "/../" : "/";
    configFile = new QFile(QCoreApplication::applicationDirPath() + QString("%1/saved_params.json").arg(pathToFile));
    configFile->open(QIODevice::ReadWrite);
    QByteArray saveData = configFile->readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
    ConfJson = jsonDocument.object();

//    QFile styleFile(QCoreApplication::applicationDirPath() + QString("%1/qss/appLogWidget.css").arg(pathToFile));
//    styleFile.open(QIODevice::ReadOnly);
//    styleFile.setFileName(QCoreApplication::applicationDirPath() + QString("%1/qss/main.css").arg(pathToFile));
//    styleFile.open(QIODevice::ReadOnly);
//    this->setStyleSheet(styleFile.readAll());
//    styleFile.close();
}

void MainWindow::checkServicesConnection(){
    logViewer->isDataBaseOk() ? ui->dbLabel->setStyleSheet("color : green") : ui->dbLabel->setStyleSheet("color : red");
}

void MainWindow::AddToLog(const QString& string, bool isError){
    if(string.isEmpty()) return;
    ui->appLogs->addItem(QTime::currentTime().toString("HH:mm:ss:zzz").append(" : ").append(string));

    ui->appLogs->item(ui->appLogs->count() - 1)->
            setForeground(isError ? QBrush(QColor("red")) : QBrush(QColor("green")));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ContextMenuRequested(const QPoint& pos){
    QModelIndex index = ui->treeView->indexAt(pos);
    if(index.isValid())
    {
        LogItem* item = treeModel->getItem(index);
        auto* menu = new QMenu(this);
        menu->setAttribute(Qt::WA_DeleteOnClose);
        auto* addGraph = new QAction( this);
        item->isActive() ? addGraph->setText("Hide Graph") : addGraph->setText("AddGraph");

        auto* deleteParam = new QAction(QString("Delete"), this);
        auto* individualAxes = new QAction(QString("Axes On/Off"), this);

        auto* configParam = new QMenu(QString("Config"), this);
        auto* changeColor = new QAction(QString("Change Line Color"), this);
        auto* changeWidth = new QAction(QString("Change Line Width"), this);
//        auto* changeStyle = new QAction(QString("Change Line Style"), this);

        menu->addAction(addGraph);
        if(item->isIamContainer()){
            menu->addAction(individualAxes);
        }
        else{
            if(item->isActive()){
                menu->addMenu(configParam);
                configParam->addAction(changeColor);
                configParam->addAction(changeWidth);
//                configParam->addAction(changeStyle);
            }
        }
        menu->addAction(deleteParam);

        QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
        QVector<LogItem*> selectedItems;
        if (!indexes.empty()){
            selectedItems.reserve(indexes.size());
            selectedItems = treeModel->getSelections(indexes);
        }

        connect(addGraph, &QAction::triggered, this, [this, selectedItems](){
            for(auto* selItem: selectedItems){
                if(selItem->isActive()){
                    logViewer->removeGraph(selItem);
                    continue;
                }
                selItem->setIsActive(!selItem->isActive());
            }
            logViewer->rebuildLayout();
        });

        connect(deleteParam, &QAction::triggered, this, [this, selectedItems](){
            treeModel->beginResetMe();
            for(auto* selItem: selectedItems)
                lastSource == LogViewerItems::DataBase ?
                    logViewer->removeItem(selItem) :
                    logViewer->moveItem(selItem);
            treeModel->endResetMe();
            logViewer->rebuildLayout();
        });

        connect(individualAxes, &QAction::triggered, this, [this, item](){
            item->setValueAxesOnOff();
            logViewer->rebuildLayout();
        });
        menu->popup(ui->treeView->viewport()->mapToGlobal(pos));

        connect(changeColor, &QAction::triggered, this, [this, selectedItems](){
            for(auto* selItem: selectedItems)
                selItem->changeColor();
            logViewer->rebuildLayout();
        });
        connect(changeWidth, &QAction::triggered, this, [this, selectedItems](){
            for(auto* selItem: selectedItems)
                selItem->changeLineWidth();
            logViewer->rebuildLayout();
        });
//        connect(changeStyle, &QAction::triggered, this, [this, selectedItems](){
//            for(auto* selItem: selectedItems)
//                selItem->changeLineStyle();
//            logViewer->rebuildLayout();
//        });
    }
}

void MainWindow::saveAll(){
//    logViewer->saveParams();
    QJsonDocument doc;
    doc.setObject(ConfJson);
    if(!configFile->isOpen()) {
        auto pathToFile = QString(CURRENT_BUILD_TYPE_) == "Debug" ? "/../" : "/";
        configFile = new QFile(QCoreApplication::applicationDirPath() + QString("%1/saved_params.json").arg(pathToFile));
        configFile->open(QIODevice::ReadWrite);
    }
    configFile->resize(0);
    configFile->write(doc.toJson(QJsonDocument::Indented));
    configFile->close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveAll();
//    logViewer->closeAll();
//    configFile->close();
//    for(auto it=setParamValueDlgMap.begin(); it!=setParamValueDlgMap.end(); it++){
//        it.value()->setAttribute(Qt::WA_DeleteOnClose);
//        it.value()->close();
//    }
    close();
}