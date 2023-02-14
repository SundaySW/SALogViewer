//
// Created by 79162 on 07.01.2023.
//

#ifndef PROTOSLOGVIEWER_LOGITEM_H
#define PROTOSLOGVIEWER_LOGITEM_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include "QVariant"
#include "view_treeview/TreeItem.h"
#include "QRandomGenerator"
#include <QBrush>
#include <qcustomplot/qcustomplot.h>
#include "QColor"
#include "QSet"
#include "main.h"

namespace LogViewerItems{
    enum Type: int{
        Regular,
        Container,
        Root
    };
}

class LogItem : public QObject, public TreeItem<LogItem>{
Q_OBJECT
public:
    LogItem();
    explicit LogItem(const QVector<QVariant>& _data, LogItem *parentItem = nullptr);
    const QString &getName() const;
    const QString &getTableName() const;
    QColor getColor();
    int getLineWidth() const;
    QCPGraph::LineStyle getLineStyle() const;
    QCPAxisRect* getRect();
    QCPGraph *getGraph();
    QCPAxis* getLabelAxis();
    QVector<QCPGraphData>& getGraphData();
    QSet<QString> getSetOfAllTabNames();
    bool contains(LogItem*);

    bool setData(int column, const QVariant &value);
    void setAsContainer();
    void setValueAxis(QCPAxis*);
    void setIsActive(bool isActive);
    void setValueAxesOnOff();

    bool bindGraph(QCPAxisRect *rect, QCustomPlot *Plot, QCPAxis *axis);
    QVariant data(int column, int role) const;
    LogItem *moveChildren(int position, LogItem *newParent);
    QCPAxisRect *removeRect();
    void bindChildGraph(QCustomPlot *Plot, LogItem *child);
    bool unBindChildGraph(QCustomPlot *Plot, LogItem *child);

    bool isActive() const;
    bool isIamUnderRoot();
    bool isIamRoot();
    bool isIamContainer();
    bool needRectWithAxes();
    bool isRightAxis();
    void changeColor();
    void changeLineWidth();
    void changeLineStyle();
private:
    QCPGraph* selfGraph = nullptr;
    QCPAxisRect* selfRect = nullptr;
    QVector<QCPGraphData> graphData;
    QString name;
    QString tableName = nullptr;
    bool active = false;
    bool rightAxis = false;
    bool withValueAxis = false;
    LogViewerItems::Type itemType = LogViewerItems::Regular;
    QCPAxis* valueAxis;
    QColor itemColor;
    int lineWidth = 2;
    QCPGraph::LineStyle lineStyle = QCPGraph::lsLine;
    QColor generateColor();
    inline static int count = 0;
};

#endif //PROTOSLOGVIEWER_LOGITEM_H