//
// Created by 79162 on 07.01.2023.
//

#include "LogViewer.h"
#include <QtWidgets/QMessageBox>
#include <LogItem/measurementview.h>
#include "QMenu"

LogViewer::LogViewer(QJsonObject &inJson, QCustomPlot* inPlot, LogItem* root, QWidget* _parent):
     mainWin(_parent)
    ,qJsonObject(inJson)
    ,dbDriver(PSQL_Driver(inJson))
    ,Plot(inPlot)
    ,rootItem(root)
{
    dbDriver.errorInDBDriver = [this](const QString& errorStr){ errorInDBToLog(errorStr);};
    dbDriver.eventInDBDriver = [this](const QString& eventStr){ eventInDBToLog(eventStr);};
    if(dbDriver.isAutoConnect()) dbDriver.setConnection();
    setPlot();
}

PSQL_Driver& LogViewer::getDbDriver() {
    return dbDriver;
}

bool LogViewer::isDataBaseOk() {
    return dbDriver.isDBOk();
}

bool LogViewer::setPlot(){
    Plot->plotLayout()->clear();
//    Plot->setOpenGl(true);
    marginGroup = new QCPMarginGroup(Plot);
    Plot->setBackground(QPixmap(QCoreApplication::applicationDirPath() + QString("/../Resources/background.png")));
    Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes | QCP::iMultiSelect);
    Plot->setContextMenuPolicy(Qt::CustomContextMenu);
    Plot->plotLayout()->setFillOrder(QCPLayoutGrid::FillOrder::foRowsFirst);
    Plot->plotLayout()->setRowSpacing(0);

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
        for(auto* selectedGraph : Plot->selectedGraphs()) {
            if(!selectedGraph->selection().isEmpty()){
                if(savedGraphs.contains(selectedGraph)){
                    savedGraphs.value(selectedGraph)->setText();
                }else
                    savedGraphs.insert(selectedGraph, new SavedGraphColor(selectedGraph->pen(), selectedGraph, Plot));
            }
        }
        for(auto it = savedGraphs.begin(); it != savedGraphs.end(); it++){
            if(it.value()->savedGraph->selection().isEmpty()){
                savedGraphs.remove(it.key());
                delete it.value();
            }
        }
    });
//    connect(Plot, &QCustomPlot::plottableDoubleClick, [this](QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent *event){
//        plottable->setSelectable(QCP::SelectionType::stMultipleDataRanges);
//        QString label = plottable->keyAxis()->label();
//        QString yValue = QString("Value is: %1").arg(plottable->valueAxis()->graphs().value(0)->dataMainValue(dataIndex));
//        auto resBtn = QMessageBox::question( mainWin, "Rect Double Click Event",
//                                             QString("You have clicked at " + label +" at X = " + yValue + "\n"),
//                                             QMessageBox::Ok);
//        if (resBtn == QMessageBox::Ok) return;
//    });
//    connect(Plot, &QCustomPlot::mousePress, [this](QMouseEvent*  event){
//         if(QGuiApplication::keyboardModifiers().testFlag(Qt::AltModifier)){
//            for(auto* item : rootItem->getMChildItems())
//                if(item->isIamContainer()) return;
//            Plot->setSelectionRectMode(QCP::srmSelect);
//        }
//        else
//            Plot->setSelectionRectMode(QCP::srmNone);
//    });
//    connect(Plot, &QCustomPlot::mouseRelease, [this](QMouseEvent*  event){
//    });

    return true;
}

void LogViewer::insertGraph(LogItem *item, bool last){
    if(!Plot->axisRectCount())
        insertRight = false;
    else
        insertRight = !insertRight;
    auto* rect = prepareRect(last);
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
        axis->setPadding(0);
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
        if(graph != nullptr)
            Plot->plotLayout()->remove(rect);
        if(rect != nullptr)
            Plot->removeGraph(graph);
        item->setIsActive(false);
    }
}

QCPAxisRect* LogViewer::prepareRect(bool last) {
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
    rect->setMarginGroup(QCP::msAll, marginGroup);

    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeFormat("MM.dd-hh:mm");
    bottomAxis->setTicker(dateTimeTicker);

    topAxis->setPadding(0);
    topAxis->setOffset(0);
    topAxis->setTicks(false);
    topAxis->setTickLabels(false);
    topAxis->setSubTicks(false);
    topAxis->setBasePen(Qt::NoPen);

    bottomAxis->setPadding(0);
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
    QPen tickerPen = QPen(QColor(LogVieverColor_white), 2, Qt::SolidLine);
    QFont bottomTickFont = QFont("Rubik", 9, 4);
    bottomAxis->setTickLabelColor(QColor(LogVieverColor_white));
    bottomAxis->setTickPen(tickerPen);
    bottomAxis->setTickLabelFont(bottomTickFont);
}

void LogViewer::loadItemFromDB(LogItem* item){
    auto& itemData = item->getGraphData();
    auto keyColumn = qJsonObject.value("DBConfObject")["KeyColumnName"].toString();
    auto valueColumn = qJsonObject.value("DBConfObject")["ValueColumnName"].toString();
    QVector<QVariant>keysVector;
    QVector<QVariant>valueVector;
    dbDriver.getLogItemData(item->getTableName(), keysVector, keyColumn);
    dbDriver.getLogItemData(item->getTableName(), valueVector, valueColumn);
    if(keysVector.size() != valueVector.size()){
        auto resBtn = QMessageBox::question( mainWin, "Problem while loading DataBase Data",
                tr("Key data size not equal value data size\n"),
                QMessageBox::Ok);
    }
    itemData.clear();
    itemData.resize(keysVector.size());
    for (int i = 0; i < keysVector.size(); ++i)
    {
        itemData[i].key = QCPAxisTickerDateTime::dateTimeToKey(keysVector[i].toDateTime().toUTC());
        itemData[i].value = valueVector[i].toDouble();
    }
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