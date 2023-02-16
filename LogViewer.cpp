//
// Created by 79162 on 07.01.2023.
//

#include "LogViewer.h"
#include <QtWidgets/QMessageBox>
#include <LogItem/measurementview.h>
#include "QMenu"

LogViewer::LogViewer(QJsonObject &inJson, QCustomPlot* inPlot, LogItem* root, QWidget* parent):
    QWidget(parent)
    ,mainWin(parent)
    ,qJsonObject(inJson)
    ,dbDriver(QSharedPointer<PSQL_Driver>(new PSQL_Driver(inJson)))
    ,Plot(inPlot)
    ,rootItem(root)
{
    dbDriver->errorInDBDriver = [this](const QString& errorStr){ errorInDBToLog(errorStr);};
    dbDriver->eventInDBDriver = [this](const QString& eventStr){ eventInDBToLog(eventStr);};
    if(dbDriver->isAutoConnect()) dbDriver->setConnection();
    setPlot();
}

QSharedPointer<PSQL_Driver> LogViewer::getDbDriver() {
    return dbDriver;
}

bool LogViewer::isDataBaseOk() {
    return dbDriver->isDBOk();
}

bool LogViewer::setPlot(){

    Plot->setOpenGl(true);
    Plot->setNotAntialiasedElement(QCP::AntialiasedElement::aeAxes);
    Plot->plotLayout()->clear();
    marginGroup = new QCPMarginGroup(Plot);
    Plot->setBackground(QPixmap(QCoreApplication::applicationDirPath() + QString("/../Resources/background.png")));
    Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes | QCP::iMultiSelect);
    Plot->setContextMenuPolicy(Qt::CustomContextMenu);
    Plot->plotLayout()->setFillOrder(QCPLayoutGrid::FillOrder::foRowsFirst);
    Plot->plotLayout()->setRowSpacing(0);
    qApp->setStyleSheet("QToolTip { background-color: #45494A; color: #BBBBBB; border: none; font-family: Rubik;}"); //todo change with

    connect(Plot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(plotContextMenuRequest(QPoint)));
    connect(Plot, &QCustomPlot::axisDoubleClick, [this](QCPAxis* axis){
        axis->rescale();
        if(axis->axisType()==QCPAxis::atRight)
            axis->axisRect()->axis(QCPAxis::atLeft)->rescale();
        Plot->replot();
    });
    connect(Plot, &QCustomPlot::mouseWheel, [this](QWheelEvent* event){
//        if(Plot->selectedAxes().contains(Plot->axisRectAt(event->position())->axis(QCPAxis::atBottom)))
//            Plot->axisRectAt(event->position())->setRangeZoom(Qt::Horizontal);
//        else
//            Plot->axisRectAt(event->position())->setRangeZoom(Qt::Vertical);
        if(QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
            Plot->axisRectAt(event->position())->setRangeZoom(Qt::Horizontal);
        else
            Plot->axisRectAt(event->position())->setRangeZoom(Qt::Vertical);
    });

    connect(Plot, &QCustomPlot::selectionChangedByUser, [this](){
        for(auto* selectedGraph : Plot->selectedGraphs()){
            if(!selectedGraph->selection().isEmpty()){
                if(savedGraphs.contains(selectedGraph)){
                    savedGraphs.value(selectedGraph)->setText();
                }else
                    savedGraphs.insert(selectedGraph, QSharedPointer<SavedGraphColor>(new SavedGraphColor(selectedGraph->pen(), selectedGraph, Plot)));
            }
        }
        for(auto it = savedGraphs.begin(); it != savedGraphs.end();){
            if(it.value()->savedGraph->selection().isEmpty()){
                it.value()->beforeDel();
                savedGraphs.erase(it++);
            }
            else it++;
        }
    });

    connect(Plot, &QCustomPlot::mouseMove, [this](QMouseEvent* event){
        if(QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)) {
            if(!savedXPos)savedXPos = event->pos().x();
            if(QGuiApplication::keyboardModifiers().testFlag(Qt::AltModifier))
                savedXPos = event->pos().x();
            auto* rect = Plot->axisRectAt(event->pos());
            auto cur = cursor();
            cur.setPos(Plot->mapToGlobal(QPoint(savedXPos, event->pos().y())));
            setCursor(cur);
            if(rect){
                double coord = rect->axis(QCPAxis::atBottom)->pixelToCoord(event->pos().x());
                double key = rect->graphs().first()->data()->findBegin(coord)->key;
                double y = rect->graphs().first()->data()->findBegin(coord)->value;
                QToolTip::showText(Plot->mapToGlobal(event->pos()),
                                   QString("Key: %1\nValue: %2")
                                   .arg(QDateTime::fromSecsSinceEpoch(key).toString("hh:mm:ss"))
                                   .arg(y));
            }
        }

    });
    connect(Plot, &QCustomPlot::mousePress, [this](QMouseEvent*  event){
        if(QGuiApplication::keyboardModifiers().testFlag(Qt::AltModifier)){
            for(auto* item : rootItem->getMChildItems())
                if(item->isIamContainer()) return;
            Plot->setSelectionRectMode(QCP::srmSelect);
        }
        else
            Plot->setSelectionRectMode(QCP::srmNone);
    });
    return true;
}

void LogViewer::insertGraph(LogItem *item, bool last){
    if(!Plot->axisRectCount())
        insertRight = false;
    else
        insertRight = !insertRight;
    auto* rect = prepareRect();
    auto *valueAxis = insertRight ? rect->axis(QCPAxis::atRight) : rect->axis(QCPAxis::atLeft);
    item->bindGraph(rect, Plot, valueAxis);
    if(item->isIamContainer())
        finishContainerRect(item);
    Plot->plotLayout()->addElement(rect);
}

void LogViewer::finishContainerRect(LogItem *item){
    if(item->needRectWithAxes()){
        auto* rect = item->getRect();
        rect->setRangeZoomAxes(rect->axes(QCPAxis::atBottom));
        rect->setRangeDrag(Qt::Horizontal);
        auto* axis = item->isRightAxis() ? rect->axis(QCPAxis::atRight) : rect->axis(QCPAxis::atLeft);
        axis->setOffset(0);
        axis->setTickLabels(false);
        axis->setBasePen(Qt::NoPen);
        axis->setTickPen(Qt::NoPen);
        axis->setSubTickPen(Qt::NoPen);
    }
}

void LogViewer::rebuildLayout(){
    auto savedRange = Plot->xAxis ? Plot->xAxis->range() : QCPRange();
    Plot->plotLayout()->clear();
    Plot->clearPlottables();
    Plot->clearGraphs();
    Plot->clearMask();
    auto& list = rootItem->getMChildItems();
    if(!list.empty()){
        auto *last = list.at(0);
        for (int i = 0; i < rootItem->childCount(); i++) {
            auto *item = list.at(i);
            if (!item->isActive())
                continue;
            insertGraph(item);
            last = item;
            for (int z = 0; z < item->childCount(); z++) {
                auto *innerItem = item->getMChildItems().at(z);
                if (innerItem->isActive())
                    item->bindChildGraph(Plot, innerItem);
            }
        }
        completeLastRect(last->getRect());
    }
    if(Plot->xAxis)
        Plot->xAxis->setRange(savedRange);
    Plot->plotLayout()->simplify();
    Plot->replot();
}

void LogViewer::removeItem(LogItem *item){
    if(item->isIamContainer())
        for(auto* childItem : item->getMChildItems())
            item->moveChildren(childItem->childNumber(), rootItem);
    removeGraph(item);
    item->parentItem()->removeChildren(item->childNumber());
}

void LogViewer::removeGraph(LogItem *item){
    if(item->isActive()){
        auto* rect = item->removeRect();
        auto* graph = item->getGraph();
        if(rect != nullptr)
            Plot->removeGraph(graph);
        if(graph != nullptr)
            Plot->plotLayout()->remove(rect);
        item->setIsActive(false);
    }
}

QCPAxisRect* LogViewer::prepareRect() {
    auto *rect = new QCPAxisRect(Plot);
    auto* bottomAxis = rect->axis(QCPAxis::atBottom);
    auto* topAxis = rect->axis(QCPAxis::atTop);
    auto* rightAxis = rect->axis(QCPAxis::atRight);
    auto* leftAxis = rect->axis(QCPAxis::atLeft);
    rect->setRangeZoom(Qt::Vertical);
    rect->setRangeZoomAxes(rect->axes());
    connect(rightAxis, SIGNAL(rangeChanged(QCPRange)), leftAxis, SLOT(setRange(QCPRange)));
    connect(Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), bottomAxis, SLOT(setRange(QCPRange)));
    connect(bottomAxis, SIGNAL(rangeChanged(QCPRange)), Plot->xAxis, SLOT(setRange(QCPRange)));
    rect->setupFullAxesBox(true);
    rect->setMargins(QMargins(0, 0, 0, 0));
    rect->setMarginGroup(QCP::msLeft|QCP::msRight, marginGroup);
    rect->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);

    topAxis->setOffset(0);
    topAxis->setTicks(false);
    topAxis->setTickLabels(false);
    topAxis->setSubTicks(false);
    topAxis->setBasePen(Qt::NoPen);

    bottomAxis->setOffset(0);
    bottomAxis->setTickLabels(false);
    bottomAxis->setBasePen(Qt::NoPen);
    bottomAxis->setTickPen(Qt::NoPen);
    bottomAxis->setSubTickPen(Qt::NoPen);

    if(insertRight){
        leftAxis->setTickLabels(false);
        leftAxis->setTickPen(Qt::NoPen);
        leftAxis->setSubTickPen(Qt::NoPen);
        leftAxis->setBasePen(Qt::NoPen);
        rightAxis->setTickLabels(true);
        rightAxis->setTicks(true);
        rightAxis->setSubTicks(true);
    }else{
        rightAxis->setTickLabels(false);
        rightAxis->setSubTicks(false);
        rightAxis->setTickLength(0);
        rightAxis->setBasePen(Qt::NoPen);
    }
    QPen gridPen = QPen(QColor(LogVieverColor_grid), 1, Qt::SolidLine);
    QPen subGridPen = QPen(QColor(LogVieverColor_subgrid), 1, Qt::SolidLine);
    for(auto *axis : rect->axes()) {
        axis->setLayer("axes");
        axis->grid()->setLayer("grid");
        axis->grid()->setSubGridVisible(true);
        axis->grid()->setZeroLinePen(Qt::NoPen);
        axis->grid()->setPen(gridPen);
        axis->grid()->setSubGridPen(subGridPen);
    }
    return rect;
}

void LogViewer::completeLastRect(QCPAxisRect *rect){
    if(rect == nullptr) return;
    auto* bottomAxis = rect->axis(QCPAxis::atBottom);
    bottomAxis->setSubTickLength(5);
    bottomAxis->setTickLength(10);
    bottomAxis->setTickLabels(true);
    bottomAxis->setTickPen(QPen(QColor(LogVieverColor_white)));
    bottomAxis->setBasePen(QPen(QColor(LogVieverColor_white),2));
    bottomAxis->setSubTickPen(QPen(QColor(LogVieverColor_white)));

    if(keyType == LogViewerItems::DateTime){
        QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
        dateTimeTicker->setDateTimeFormat("MM.dd-hh:mm");
        bottomAxis->setTicker(dateTimeTicker);
    }
    else if(keyType == LogViewerItems::mSecs){
        QSharedPointer<MsecTicker> msecTicker(new MsecTicker);
        bottomAxis->setTicker(msecTicker);
    }else{

    }

    QPen tickerPen = QPen(QColor(LogVieverColor_white), 2, Qt::SolidLine);
    QFont bottomTickFont = QFont("Rubik", 9, 4);
    bottomAxis->setTickLabelColor(QColor(LogVieverColor_white));
    bottomAxis->setTickPen(tickerPen);
    bottomAxis->setTickLabelFont(bottomTickFont);
}

void LogViewer::plotContextMenuRequest(const QPoint &pos){
    if(!Plot->selectedPlottables().isEmpty()) {
        auto *menu = new QMenu(mainWin);
        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->addAction("Make measurement", [this, pos]() {
            makeMeasurementDlg(pos);
        });
        menu->popup(Plot->mapToGlobal(pos));
    }
}

void LogViewer::makeMeasurementDlg(const QPoint &pos){
    auto selGraphs = Plot->selectedGraphs();
    for(auto* graph : selGraphs){
        auto dataRange = graph->selection().dataRange();
        auto graphData = graph->data();
        auto* dlg = new QDialog(mainWin);
        QVector<QCPGraphData> newData;
        newData.reserve(dataRange.size());
        for(auto it = graphData->at(dataRange.begin()); it != graphData->at(dataRange.end()); ++it)
            newData.append(QCPGraphData(it->key, it->value));
        LogItem* currentItem;
        for(auto* item: rootItem->getMChildItems()){
            if(item->getGraph() == graph){
                currentItem = item;
                break;
            }
            for(auto* innerItem: item->getMChildItems())
                if(innerItem->getGraph() == graph){
                    currentItem = item;
                    break;
                }
        }
        auto* measurementFrame = new MeasurementView(newData, currentItem, dlg);
        dlg->setWindowTitle(QString("Measurement for %1").arg(currentItem->getName()));
        auto* diagLayout = new QVBoxLayout();
        diagLayout->addWidget(measurementFrame);
        dlg->setLayout(diagLayout);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
    }
}

bool LogViewer::manageOpenGl() {
    bool retVal;
    Plot->setOpenGl(!Plot->openGl());
    retVal = Plot->openGl();
    Plot->setNoAntialiasingOnDrag(!retVal);
    return retVal;
}

void LogViewer::setKeyAxisDataType(LogViewerItems::KeyType newType){
    keyType = newType;
}

QJsonObject& LogViewer::getQJsonObject()const{
    return qJsonObject;
}

LogViewerItems::KeyType LogViewer::getKeyType() const {
    return keyType;
}
