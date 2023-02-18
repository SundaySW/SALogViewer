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
    explicit MeasurementView(const QVector<QCPGraphData> &graphData, const QColor& itemColor, QWidget *parent);
    void setPlot(const QVector<QCPGraphData> &graphData, const QColor& itemColor);
    void setBottomAxis(LogViewerItems::KeyType keyType);
    ~MeasurementView() override;
private:
    Ui::MeasurementView *ui;
    QCustomPlot* Plot;
    QCPAxisRect* selfRect;
    void countSetValues(const QVector<QCPGraphData> &graphData);
};

#endif // MEASUREMENTVIEW_H
