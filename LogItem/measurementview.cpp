#include "measurementview.h"
#include "ui_measurementview.h"
#include "main.h"

MeasurementView::MeasurementView(const QVector<QCPGraphData> &graphData, const QColor& itemColor, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeasurementView)
{
    ui->setupUi(this);
    Plot = ui->selPlot;
    setPlot(graphData, itemColor);
    countSetValues(graphData);
    Plot->replot();
}

MeasurementView::~MeasurementView()
{
    delete ui;
}

void MeasurementView::setBottomAxis(LogViewerItems::KeyType keyType){
    auto* bottomAxis = selfRect->axis(QCPAxis::atBottom);
    LogViewerItems::bindTickerToAxis(bottomAxis, keyType);
    bottomAxis->setTickLabelRotation(30);
    bottomAxis->setSubTickLength(3);
    bottomAxis->setTickLength(7);
    bottomAxis->setTickLabels(true);
    bottomAxis->setTickPen(QPen(QColor(LogVieverColor_white)));
    bottomAxis->setBasePen(QPen(QColor(LogVieverColor_white),1));
    bottomAxis->setSubTickPen(QPen(QColor(LogVieverColor_white)));
    QPen tickerPen = QPen(QColor(LogVieverColor_white), 1, Qt::SolidLine);
    bottomAxis->setTickLabelColor(QColor(LogVieverColor_white));
    bottomAxis->setTickPen(tickerPen);
}

void MeasurementView::countSetValues(const QVector<QCPGraphData> &graphData) {
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

void MeasurementView::setPlot(const QVector<QCPGraphData> &graphData, const QColor& itemColor) {
    Plot->plotLayout()->clear();
    Plot->setBackground(QPixmap(QCoreApplication::applicationDirPath() + QString("/../Resources/background.png")));

    selfRect = new QCPAxisRect(Plot);
    auto* topAxis = selfRect->axis(QCPAxis::atTop);
    auto* leftAxis = selfRect->axis(QCPAxis::atLeft);

    topAxis->setTicks(false);
    topAxis->setTickLabels(false);
    topAxis->setSubTicks(false);
    topAxis->setBasePen(Qt::NoPen);

    QPen gridPen = QPen(QColor(LogVieverColor_grid), 1, Qt::SolidLine);
    QPen subGridPen = QPen(QColor(LogVieverColor_subgrid), 1, Qt::SolidLine);
    for(auto *axis : selfRect->axes()) {
        axis->setLayer("axes");
        axis->grid()->setLayer("grid");
        axis->grid()->setSubGridVisible(true);
        axis->grid()->setZeroLinePen(Qt::NoPen);
        axis->grid()->setPen(gridPen);
        axis->grid()->setSubGridPen(subGridPen);
    }

    auto selfGraph = Plot->addGraph(selfRect->axis(QCPAxis::atBottom), selfRect->axis(QCPAxis::atLeft));
    selfGraph->data()->set(graphData);

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
    axis->setLabelColor(itemColor);
    axis->setTickLabelColor(itemColor);

    selfGraph->rescaleValueAxis();
    selfGraph->rescaleKeyAxis();
    Plot->plotLayout()->addElement(selfRect);
}
