#ifndef MEASUREMENTVIEW_H
#define MEASUREMENTVIEW_H

#include <QFrame>
#include <qcustomplot/qcustomplot.h>
#include "LogItem.h"

namespace Ui {
class MeasurementView;
}

class MeasurementView : public QWidget
{
    Q_OBJECT

public:
    explicit MeasurementView(const QVector<QCPGraphData>&, LogItem*, QWidget* = nullptr);
    ~MeasurementView();
    void setPlot(const QVector<QCPGraphData> &graphData, LogItem *item);

private:
    Ui::MeasurementView *ui;
    QCustomPlot* Plot;
    void countSetValues(const QVector<QCPGraphData> &graphData, LogItem *item);
};

#endif // MEASUREMENTVIEW_H
