#include "measurementview.h"
#include "ui_measurementview.h"
#include "main.h"

MeasurementView::MeasurementView(const QVector<QCPGraphData> &graphData, LogItem *item, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeasurementView)
{
    ui->setupUi(this);
    Plot = ui->selPlot;
    setPlot(graphData, item);
    countSetValues(graphData, item);
    Plot->replot();
}

MeasurementView::~MeasurementView()
{
    delete ui;
}

void MeasurementView::countSetValues(const QVector<QCPGraphData> &graphData, LogItem *item){
    double min = graphData.first().value;
    double max = graphData.first().value;
    double avg = 0;
    for(auto data : graphData){
        auto value = data.value;
        if(max < value) max = value;
        if(min > value) min = value;
        avg += value;
    }
    ui->maxValue->setText(QString("%1").arg(max));
    ui->minValue->setText(QString("%1").arg(min));
    ui->avgValue->setText(QString("%1").arg(avg/graphData.size()));
}

void MeasurementView::setPlot(const QVector<QCPGraphData> &graphData, LogItem *item) {
    Plot->plotLayout()->clear();
    Plot->setBackground(QPixmap(QCoreApplication::applicationDirPath() + QString("/../Resources/background.png")));

    auto *rect = new QCPAxisRect(Plot);
    auto* bottomAxis = rect->axis(QCPAxis::atBottom);
    auto* topAxis = rect->axis(QCPAxis::atTop);
    auto* leftAxis = rect->axis(QCPAxis::atLeft);

    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeFormat("MM.dd\nhh:mm:ss");
    bottomAxis->setTicker(dateTimeTicker);
//    bottomAxis->setTickLabelRotation(30);

    topAxis->setPadding(0);
    topAxis->setOffset(0);
    topAxis->setTicks(false);
    topAxis->setTickLabels(false);
    topAxis->setSubTicks(false);
    topAxis->setBasePen(Qt::NoPen);

    bottomAxis->setSubTickLength(3);
    bottomAxis->setTickLength(7);
    bottomAxis->setTickLabels(true);
    bottomAxis->setTickPen(QPen(QColor(LogVieverColor_white)));
    bottomAxis->setBasePen(QPen(QColor(LogVieverColor_white),1));
    bottomAxis->setSubTickPen(QPen(QColor(LogVieverColor_white)));
    QPen tickerPen = QPen(QColor(LogVieverColor_white), 1, Qt::SolidLine);
    bottomAxis->setTickLabelColor(QColor(LogVieverColor_white));
    bottomAxis->setTickPen(tickerPen);

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

    auto selfGraph = Plot->addGraph(rect->axis(QCPAxis::atBottom), rect->axis(QCPAxis::atLeft));
    selfGraph->data()->set(graphData);

    auto itemColor = item->getColor();
    QPen graphPen = QPen(itemColor,2);
    QPen axisPen = QPen(itemColor);
    selfGraph->setLineStyle(QCPGraph::LineStyle::lsLine);
    selfGraph->setPen(graphPen);
    auto brushColor = itemColor;
    brushColor.setAlpha(50);
    selfGraph->setBrush(QBrush(brushColor));
    auto* axis = leftAxis;
    axis->setBasePen(axisPen);
    axis->setSubTickPen(axisPen);
    axis->setTickPen(axisPen);
    axis->setLabel(item->getName());
    axis->setLabelColor(itemColor);
    axis->setTickLabelColor(itemColor);

    selfGraph->rescaleValueAxis();
    selfGraph->rescaleKeyAxis();
    Plot->plotLayout()->addElement(rect);
}
