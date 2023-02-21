//
// Created by 79162 on 07.01.2023.
//

#ifndef PROTOSLOGVIEWER_LOGVIEWER_H
#define PROTOSLOGVIEWER_LOGVIEWER_H

#include <DB_Driver/PSQL_Driver.h>
#include <LogItem/LogItem.h>
#include <qcustomplot/qcustomplot.h>
#include "CustomTicker/MsecTicker.h"

#include <utility>
#include <LogItem/tooltipframe.h>
#include "QWidget"

class LogViewer: public QWidget{
Q_OBJECT
public:
    explicit LogViewer(QJsonObject&, QCustomPlot*, LogItem* root, LogItem* stored, QWidget* _parent = nullptr);
    QSharedPointer<PSQL_Driver> getDbDriver();
    bool isDataBaseOk();
    void insertGraph(LogItem*, bool last = false);
    void removeGraph(LogItem*);
    void removeItem(LogItem*);
    void moveItem(LogItem *item);
    void rebuildLayout();
    bool manageOpenGl();
    QJsonObject& getQJsonObject() const;
    LogViewerItems::KeyType getKeyType() const;
    void setKeyAxisDataType(LogViewerItems::KeyType);
signals:
    void errorInDBToLog(const QString&);
    void eventInDBToLog(const QString&);
private slots:
    void plotContextMenuRequest(const QPoint& pos);
private:
    QSharedPointer<PSQL_Driver> dbDriver;
    QWidget* mainWin;
    QJsonObject& qJsonObject;
    QCustomPlot* Plot;
    LogItem* rootItem;
    LogItem* storedRoot;
    QCPMarginGroup* marginGroup;
    inline static int count;
    int savedXPos;
    bool insertRight = false;
    ToolTipFrame* toolTipFrame;
    QVector<LogViewerItems::ToolTipData> toolTipData;
    QMap<QCPGraph*, QSharedPointer<LogViewerItems::SavedGraphColor>> savedGraphs = {};
    LogViewerItems::KeyType keyType = LogViewerItems::DateTime;
    QCPAxisRect* prepareRect();
    bool setPlot();
    void completeLastRect(QCPAxisRect*);
    void finishContainerRect(LogItem*);
    void makeMeasurementDlg(const QPoint &pos);
    void makeToolTipData(double key);
    LogItem *getItemByGraph(QCPGraph *graph);
};
#endif //PROTOSLOGVIEWER_LOGVIEWER_H
