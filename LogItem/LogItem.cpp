//
// Created by 79162 on 07.01.2023.
//

#include "LogItem.h"

extern QRandomGenerator generator;

LogItem::LogItem()
        :TreeItem(QVector<QVariant>{"Container"}, nullptr)
        ,tableName("")
        ,name(QString("Container_%1").arg(++count))
        ,selfRect(nullptr)
        ,selfGraph(nullptr)
{
    itemColor = QColor(LogVieverColor_white);
    itemType = LogViewerItems::Container;
    active = true;
}

LogItem::LogItem(const QVector<QVariant>& _data, LogItem* parentItem)
    :TreeItem(_data, parentItem)
    ,tableName(_data.at(0).toString())
    ,name(_data.at(0).toString())
    ,selfRect(nullptr)
    ,selfGraph(nullptr)
{
    itemColor = generateColor();
//    graphData.resize(100);
//    for (int i = 0; i < graphData.size(); ++i)
//    {
//        graphData[i].key = i;
//        graphData[i].value = generator.generate()/(double)RAND_MAX-0.5+graphData[qMax(0, i-1)].value;
//    }
}

QColor LogItem::generateColor(){
    auto randColor = QColor(generator.bounded(120,255), generator.bounded(120,255), generator.bounded(120,255));
    auto newColor = QColor();
//    float value = (randColor.red() + randColor.green() + randColor.blue())/3;
//    float newValue = value + generator.bounded(50);
//    float valueRatio = newValue / value;
//    newColor.setRed(randColor.red() * valueRatio);
//    newColor.setGreen(randColor.green() * valueRatio);
//    newColor.setBlue(randColor.blue() * valueRatio);
    newColor.setRed(randColor.red());
    newColor.setGreen(randColor.green());
    newColor.setBlue(randColor.blue());
    return newColor;
}

void LogItem::changeColor(){
    itemColor = generateColor();
}

void LogItem::changeLineWidth(){
    if(selfGraph->pen().width() == 2)
        lineWidth = 1;
    else
        lineWidth = 2;
}

void LogItem::changeLineStyle(){
    if(selfGraph->lineStyle() == QCPGraph::lsLine)
        lineStyle = QCPGraph::lsNone;
    else
        lineStyle = QCPGraph::lsLine;
}

bool LogItem::setData(int column, const QVariant &value) {
    if(column == 0 && value.toString().length())
        name = value.toString();
    return TreeItem::setData(column, value);
}

LogItem* LogItem::moveChildren(int position, LogItem* newParent){
    if(position < 0 || position > m_childItems.size()) return nullptr;
    auto* movingChild = m_childItems.takeAt(position);
    if(newParent->childCount() || newParent->isIamRoot()){
        newParent->appendChild(movingChild);
        movingChild->setParentItem(newParent);
    }
    else{
        auto* container = new LogItem();
        container->m_parentItem = this;
        m_childItems.append(container);
        newParent->m_parentItem = container;
        movingChild->m_parentItem = container;
        m_childItems.removeAll(newParent);
        container->m_childItems.append(movingChild);
        container->m_childItems.append(newParent);
    }
    return movingChild;
}

QColor LogItem::getColor(){
    return itemColor;
}

const QString &LogItem::getName() const {
    return name;
}

const QString &LogItem::getTableName() const {
    return tableName;
}

QVariant LogItem::data(int column, int role) const {
    switch (role)
    {
        case Qt::BackgroundRole:
            return QBrush(QColor(LogVieverColor_dark));
        case Qt::DisplayRole:
            return TreeItem::data(column,role);
        case Qt::DecorationRole:
            if(isActive())
                return itemColor;
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
    }
    return {};
}

bool LogItem::isActive() const {
    return active;
}

void LogItem::setIsActive(bool isActive) {
    active = isActive;
}

bool LogItem::contains(LogItem* item) {
   return m_childItems.contains(item);
}

QSet<QString> LogItem::getSetOfAllTabNames(){
    QSet<QString> retVal;
    retVal.reserve(childCount());
    for(const auto* item: m_childItems){
        retVal.insert(item->tableName);
        for(const auto* innerItem: item->m_childItems)
            retVal.insert(innerItem->tableName);
    }
    return retVal;
}

QCPAxisRect* LogItem::getRect() {
    return selfRect;
}

QCPGraph* LogItem::getGraph() {
    return selfGraph;
}

QCPAxisRect * LogItem::removeRect() {
    auto retVal = selfRect;
    selfRect = nullptr;
    return retVal;
}

QVector<QCPGraphData>& LogItem::getGraphData() {
    return graphData;
}

bool LogItem::isIamRoot() {
    return parentItem() == nullptr;
}

bool LogItem::isIamContainer(){
    return itemType == LogViewerItems::Container;
}

void LogItem::setAsContainer(){
    itemType = LogViewerItems::Container;
}

bool LogItem::isIamUnderRoot() {
    return parentItem()->tableName == "root";
}

void LogItem::setValueAxis(QCPAxis* axis){
    valueAxis = axis;
}

void LogItem::setValueAxesOnOff(){
    if(itemType == LogViewerItems::Container)
        withValueAxis = !withValueAxis;
}

QCPAxis *LogItem::getLabelAxis() {
    return valueAxis;
}

bool LogItem::isRightAxis() {
    return rightAxis;
}

bool LogItem::needRectWithAxes(){
    if(itemType == LogViewerItems::Container)
        return withValueAxis;
    else
        return false;
}

int LogItem::getLineWidth() const {
    return lineWidth;
}

QCPGraph::LineStyle LogItem::getLineStyle() const {
    return lineStyle;
}

bool LogItem::bindGraph(QCPAxisRect *rect, QCustomPlot *Plot, QCPAxis* axis){
    selfRect = rect;
    rightAxis = axis->axisType() == QCPAxis::atRight;

    selfGraph = Plot->addGraph(rect->axis(QCPAxis::atBottom), rect->axis(QCPAxis::atLeft));
    selfGraph->data()->set(graphData);

    QPen graphPen = QPen(itemColor,lineWidth);
    QPen axisPen = QPen(itemColor);
    selfGraph->setLineStyle(lineStyle);
    if(selfGraph->lineStyle() == QCPGraph::lsNone)
        selfGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 7));
    selfGraph->setPen(graphPen);
    axis->setBasePen(axisPen);
    axis->setSubTickPen(axisPen);
    axis->setTickPen(axisPen);
    auto itemName = getName();
    if(name.length()>12){//todo improve!
        itemName.clear();
        for(const auto& s : name.split(" "))
            itemName += s +'\n';
    }
    axis->setLabel(itemName.trimmed());
    axis->setLabelColor(itemColor);
    axis->setTickLabelColor(itemColor);

    selfGraph->rescaleValueAxis();
    selfGraph->rescaleKeyAxis();
    selfGraph->setSelectable(QCP::SelectionType::stDataRange);
    return true;
}

void LogItem::bindChildGraph(QCustomPlot *Plot, LogItem *child) {
    QCPGraph* newChildGraph;
    if(withValueAxis){
        QCPAxis* newValueAxis = selfRect->addAxis(rightAxis? QCPAxis::atLeft : QCPAxis::atRight);
        connect(Plot, &QCustomPlot::mouseWheel, [this, newValueAxis, Plot](QWheelEvent* event){
            if(Plot->selectedAxes().contains(newValueAxis)){
                auto range = newValueAxis->range();
                double scaling = range.size()/10;
                if(event->angleDelta().y()>0)
                    newValueAxis->setRange(range.lower-scaling, range.upper+scaling);
                else
                    newValueAxis->setRange(range.lower+scaling, range.upper-scaling);
            }
        });
        connect(newValueAxis, SIGNAL(rangeChanged(QCPRange)), selfRect->axis(QCPAxis::atLeft), SLOT(setRange(QCPRange)));
        connect(selfRect->axis(QCPAxis::atLeft), SIGNAL(rangeChanged(QCPRange)), newValueAxis, SLOT(rescale()));
        connect(newValueAxis, SIGNAL(rangeChanged(QCPRange)), selfRect->axis(QCPAxis::atLeft), SLOT(rangeChanged(QCPRange)));

        auto childColor = child->getColor();
        auto childLineWidth = child->getLineWidth();
        newValueAxis->setLabel(child->getName());
        newValueAxis->setLabelColor(childColor);
        newValueAxis->setTickLabelColor(childColor);
        newValueAxis->setPadding(10);
        newValueAxis->setLabelPadding(3);
        newValueAxis->setTickLength(4,4);
        newValueAxis->setBasePen(QPen(childColor, childLineWidth));
        newValueAxis->setTickPen(QPen(childColor));
        newValueAxis->setSubTickPen(QPen(childColor));
        child->setValueAxis(newValueAxis);
        newChildGraph = Plot->addGraph(selfRect->axis(QCPAxis::atBottom), newValueAxis);
    }
    else{
        newChildGraph = Plot->addGraph(selfRect->axis(QCPAxis::atBottom), selfRect->axis(QCPAxis::atLeft));
        if(!selfRect->axes().contains(child->getLabelAxis())) {
            for (auto *item: getMChildItems()) {
                QCPAxis *newLabelAxis;
                newLabelAxis = selfRect->addAxis(rightAxis ? QCPAxis::atRight : QCPAxis::atLeft);
                newLabelAxis->setLabel(item->getName());
                newLabelAxis->setLabelColor(item->getColor());
                newLabelAxis->setTickLabelColor(item->getColor());
                newLabelAxis->setOffset(0);
                newLabelAxis->setPadding(0);
                newLabelAxis->setLabelPadding(0);
                newLabelAxis->setTickLabels(false);
                newLabelAxis->setSubTicks(false);
                newLabelAxis->setTicks(false);
                newLabelAxis->setBasePen(Qt::NoPen);
                item->setValueAxis(newLabelAxis);
            }
        }
    }
    child->selfGraph = newChildGraph;
    newChildGraph->data()->set(child->getGraphData());
    newChildGraph->setLineStyle(child->getLineStyle());
    if(newChildGraph->lineStyle() == QCPGraph::lsNone)
        newChildGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 7));
    newChildGraph->setPen(QPen(child->getColor(),child->getLineWidth()));
    newChildGraph->rescaleValueAxis();
    newChildGraph->rescaleKeyAxis();
    newChildGraph->setSelectable(QCP::SelectionType::stDataRange);
}

bool LogItem::unBindChildGraph(QCustomPlot *Plot, LogItem* child) {
    bool retVal = false;
    auto* childGraph = child->selfGraph;
    if(childGraph != nullptr)
        retVal = Plot->removeGraph(childGraph);
    return retVal;
}