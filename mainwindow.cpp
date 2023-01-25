

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
{
    ui->setupUi(this);
    openFileLoadConfig();
    logViewer = new LogViewer(ConfJson);
    settingsDlg = new Settings_dlg(logViewer, ConfJson, this);
    ui->Plot->setBackground(QBrush(QColor(0, 0, 0, 0)));
    connect(ui->settings_btn, &QPushButton::clicked, [this]{
        settingsDlg->show();
        settingsDlg->raise();
        settingsDlg->activateWindow();
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
    connect(ui->loadParams_btn, &QPushButton::clicked, [this](){

    });
    QStringList headers;
    headers << "Узел";
    treeModel = new TreeModel(headers,this);
    ui->treeView->setModel(treeModel);
//    ui->treeView->setHeaderHidden(true);
//    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
//    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
//    ui->treeView->setDefaultDropAction(Qt::MoveAction);
//    ui->treeView->setDragDropOverwriteMode(true);
//    ui->treeView->setDragDropMode(QAbstractItemView::InternalMove);
//    ui->treeView->setDragEnabled(true);
//    ui->treeView->setDropIndicatorShown(true);
//    plotTest();
    newPlots();
}

void MainWindow::openFileLoadConfig() {
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

void MainWindow::AddToLog(const QString& string, bool isError)
{
    if(string.isEmpty()) return;
    ui->appLogs->addItem(QTime::currentTime().toString("HH:mm:ss:zzz").append(" : ").append(string));

    ui->appLogs->item(ui->appLogs->count() - 1)->
            setForeground(isError ? QBrush(QColor("red")) : QBrush(QColor("green")));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newPlots(){
    QCustomPlot* customPlot = ui->Plot;
    customPlot->plotLayout()->clear(); // clear default axis rect so we can start from scratch
    customPlot->plotLayout()->setFillOrder(QCPLayoutGrid::FillOrder::foRowsFirst);
//    customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
//    customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
//    customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
//    customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
//    customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    customPlot->xAxis->setTickLabelColor(Qt::white);
//    customPlot->yAxis->setTickLabelColor(Qt::white);

//    QCPGraph *graph1 = customPlot->addGraph();
//    graph1->setData(x1, y1);
//    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
//    graph1->setPen(QPen(QColor(120, 120, 120), 2));
//
//    QCPGraph *graph2 = customPlot->addGraph();
//    graph2->setData(x2, y2);
//    graph2->setPen(Qt::NoPen);
//    graph2->setBrush(QColor(200, 200, 200, 20));
//    graph2->setChannelFillGraph(graph1);
    QCPAxisRect *rect1 = new QCPAxisRect(customPlot);
    rect1->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    rect1->axis(QCPAxis::atBottom)->setLayer("axes");
    rect1->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
    rect1->axis(QCPAxis::atRight, 0)->setTickLabels(true);

    //    wideAxisRect->addAxis(QCPAxis::atLeft)->setTickLabelColor(QColor("#6050F8")); // add an extra axis on the left and color its numbers

    QCPAxisRect *rect2 = new QCPAxisRect(customPlot);
    rect2->setupFullAxesBox(true);
//    wideAxisRect2->addAxis(QCPAxis::atRight)->setTickLabelColor(QColor("#5050F8")); // add an extra axis on the left and color its numbers

    auto *rect3 = new QCPAxisRect(customPlot, false);
    rect3->addAxis(QCPAxis::atLeft);

//    rect1->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
//    rect3->axis(QCPAxis::atBottom)->setLayer("axes");
//    rect3->axis(QCPAxis::atBottom)->grid()->setLayer("grid");

    customPlot->plotLayout()->addElement(rect1);
    customPlot->plotLayout()->addElement(rect2);
    customPlot->plotLayout()->addElement(rect3);

    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeSpec(Qt::UTC);
    dateTimeTicker->setDateTimeFormat("mm.ss:zzzz");
    rect1->axis(QCPAxis::atBottom)->setTicker(dateTimeTicker);
    rect1->axis(QCPAxis::atBottom)->setTickLabelRotation(30);
//
//    QVector<QCPGraphData> dataCos(21), dataGauss(50), dataRandom(100);
//
//    QCPGraph *mainGraph = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
//    mainGraphCos->data()->set(dataCos);
//    mainGraphCos->valueAxis()->setRange(-1, 1);
//    mainGraphCos->rescaleKeyAxis();
//    mainGraphCos->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
//    mainGraphCos->setPen(QPen(QColor(120, 120, 120), 2));
//
//    QCPGraph *nextGraph = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atRight));
//    nextGraph->data()->set(dataGauss);
//    nextGraph->setPen(QPen(QColor("#8070B8"), 2));
//    nextGraph->setBrush(QColor(110, 170, 110, 30));
//    nextGraph->setChannelFillGraph(mainGraphGauss);
//    nextGraph->setBrush(QColor(255, 161, 0, 50));
//    nextGraph->valueAxis()->setRange(0, 1000);
//    nextGraph->rescaleKeyAxis();
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect );

}

void MainWindow::plotTest() {
    QCustomPlot* customPlot = ui->Plot;

// configure axis rect:
    customPlot->plotLayout()->clear(); // clear default axis rect so we can start from scratch
    QCPAxisRect *wideAxisRect = new QCPAxisRect(customPlot);
    wideAxisRect->setupFullAxesBox(true);
    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    wideAxisRect->addAxis(QCPAxis::atLeft)->setTickLabelColor(QColor("#6050F8")); // add an extra axis on the left and color its numbers
    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    customPlot->plotLayout()->addElement(0, 0, wideAxisRect); // insert axis rect in first row
    customPlot->plotLayout()->addElement(1, 0, subLayout); // sub layout in second row (grid layout will grow accordingly)
//customPlot->plotLayout()->setRowStretchFactor(1, 2);
// prepare axis rects that will be placed in the sublayout:
    QCPAxisRect *subRectLeft = new QCPAxisRect(customPlot, false); // false means to not setup default axes
    QCPAxisRect *subRectRight = new QCPAxisRect(customPlot, false);
    subLayout->addElement(0, 0, subRectLeft);
    subLayout->addElement(0, 1, subRectRight);
    subRectRight->setMaximumSize(150, 150); // make bottom right axis rect size fixed 150x150
    subRectRight->setMinimumSize(150, 150); // make bottom right axis rect size fixed 150x150
// setup axes in sub layout axis rects:
    subRectLeft->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
    subRectRight->addAxes(QCPAxis::atBottom | QCPAxis::atRight);
    subRectLeft->axis(QCPAxis::atLeft)->ticker()->setTickCount(2);
    subRectRight->axis(QCPAxis::atRight)->ticker()->setTickCount(2);
    subRectRight->axis(QCPAxis::atBottom)->ticker()->setTickCount(2);
    subRectLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);
// synchronize the left and right margins of the top and bottom axis rects:
    QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
    subRectLeft->setMarginGroup(QCP::msLeft, marginGroup);
    subRectRight->setMarginGroup(QCP::msRight, marginGroup);
    wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
// move newly created axes on "axes" layer and grids on "grid" layer:
            foreach (QCPAxisRect *rect, customPlot->axisRects())
        {
                    foreach (QCPAxis *axis, rect->axes())
                {
                    axis->setLayer("axes");
                    axis->grid()->setLayer("grid");
                }
        }

// prepare data:
    QVector<QCPGraphData> dataCos(21), dataGauss(50), dataRandom(100);
    QVector<double> x3, y3;
    auto rand = QRandomGenerator(3);
    for (int i=0; i<dataCos.size(); ++i)
    {
        dataCos[i].key = i/(double)(dataCos.size()-1)*10-5.0;
        dataCos[i].value = qCos(dataCos[i].key);
    }
    for (int i=0; i<dataGauss.size(); ++i)
    {
        dataGauss[i].key = i/(double)dataGauss.size()*10-5.0;
        dataGauss[i].value = qExp(-dataGauss[i].key*dataGauss[i].key*0.2)*1000;
    }
    for (int i=0; i<dataRandom.size(); ++i)
    {
        dataRandom[i].key = i/(double)dataRandom.size()*10;
        dataRandom[i].value = rand.generateDouble()/(double)RAND_MAX-0.5+dataRandom[qMax(0, i-1)].value;
    }
    x3 << 1 << 2 << 3 << 4;
    y3 << 2 << 2.5 << 4 << 1.5;

// create and configure plottables:
    QCPGraph *mainGraphCos = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    mainGraphCos->data()->set(dataCos);
    mainGraphCos->valueAxis()->setRange(-1, 1);
    mainGraphCos->rescaleKeyAxis();
    mainGraphCos->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
    mainGraphCos->setPen(QPen(QColor(120, 120, 120), 2));
    QCPGraph *mainGraphGauss = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft, 1));
    mainGraphGauss->data()->set(dataGauss);
    mainGraphGauss->setPen(QPen(QColor("#8070B8"), 2));
    mainGraphGauss->setBrush(QColor(110, 170, 110, 30));
    mainGraphCos->setChannelFillGraph(mainGraphGauss);
    mainGraphCos->setBrush(QColor(255, 161, 0, 50));
    mainGraphGauss->valueAxis()->setRange(0, 1000);
    mainGraphGauss->rescaleKeyAxis();

    QCPGraph *subGraphRandom = customPlot->addGraph(subRectLeft->axis(QCPAxis::atBottom), subRectLeft->axis(QCPAxis::atLeft));
    subGraphRandom->data()->set(dataRandom);
    subGraphRandom->setLineStyle(QCPGraph::lsImpulse);
    subGraphRandom->setPen(QPen(QColor("#FFA100"), 1.5));
    subGraphRandom->rescaleAxes();

    QCPBars *subBars = new QCPBars(subRectRight->axis(QCPAxis::atBottom), subRectRight->axis(QCPAxis::atRight));
    subBars->setWidth(3/(double)x3.size());
    subBars->setData(x3, y3);
    subBars->setPen(QPen(Qt::black));
    subBars->setAntialiased(false);
    subBars->setAntialiasedFill(false);
    subBars->setBrush(QColor("#705BE8"));
    subBars->keyAxis()->setSubTicks(false);
    subBars->rescaleAxes();
// setup a ticker for subBars key axis that only gives integer ticks:
    QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
    intTicker->setTickStep(1.0);
    intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    subBars->keyAxis()->setTicker(intTicker);

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect );
//    customPlot->replot();
}

