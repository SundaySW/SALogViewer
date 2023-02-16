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
#include "QWidget"

class LogViewer: public QWidget{
Q_OBJECT
public:
    struct SavedGraphColor{
        QPen savedPen;
        QCPGraph* savedGraph;
        QCPItemText* text;
        QCustomPlot* customPlot;
        SavedGraphColor(QPen pen, QCPGraph* plottable, QCustomPlot* cPlot)
            :savedPen(pen), savedGraph(plottable), customPlot(cPlot), text(nullptr)
        {
            auto* decorator = new QCPSelectionDecorator();
            decorator->setPen(plottable->pen());
            auto alphaColor = plottable->pen().color();
            alphaColor.setAlpha(40);
            decorator->setBrush(QBrush(alphaColor));
            savedGraph->setSelectionDecorator(decorator);
            savedGraph->setPen(QPen(QColor(LogVieverColor_middle2)));
            if(savedGraph->selection().dataRange().size()>2) setText();
        }
        void setText(){
            if(savedGraph->selection().dataRange().size()<3) return;
            QCPItemText* lastItem = nullptr;
            if(!savedGraph->keyAxis()->axisRect()->items().isEmpty())
                lastItem = dynamic_cast<QCPItemText*>(savedGraph->keyAxis()->axisRect()->items().last());
            if(!text){
                text = new QCPItemText(customPlot);
                text->position->setType(QCPItemPosition::ptAxisRectRatio);
                text->position->setAxisRect(savedGraph->keyAxis()->axisRect());
                text->setClipToAxisRect(false);
                text->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
                if(lastItem) {
                    text->position->setParentAnchor(lastItem->bottom);
                    text->position->setCoords(0.1, 0);
                }else
                    text->position->setCoords(0.3, 0.9);
                text->setTextAlignment(Qt::AlignHCenter);
                QFont font = QFont("Rubik", 9, 4);
                text->setFont(font);
                text->setPen(savedPen);
                text->setColor(QColor(LogVieverColor_white));
                text->setPadding(QMargins(8, 8, 8, 8));
            }
            text->setText(calculateValues());
        }
        QString calculateValues() const{
            auto dataRange = savedGraph->selection().dataRange();
            auto graphData = savedGraph->data();
            QVector<double> newData;
            newData.reserve(dataRange.size());
            for(auto it = graphData->at(dataRange.begin()); it != graphData->at(dataRange.end()); ++it)
                newData.append(it->value);
            double min = newData.first();
            double max = newData.first();
            double avg = 0;
            for(auto data : newData){
                auto value = data;
                if(max < value) max = value;
                if(min > value) min = value;
                avg += value;
            }
            return QString("Min: %1\nMax: %2\nAvg: %3")
                    .arg(min).arg(max).arg(avg/newData.size(),0,10,3,'0');
        }
        void beforeDel(){
            savedGraph->setPen(savedPen);
            customPlot->removeItem(text);
        }
        ~SavedGraphColor(){}
    };

    explicit LogViewer(QJsonObject&, QCustomPlot*, LogItem* root, QWidget* _parent = nullptr);
    QSharedPointer<PSQL_Driver> getDbDriver();
    bool isDataBaseOk();
    void insertGraph(LogItem*, bool last = false);
    void removeGraph(LogItem*);
    void removeItem(LogItem*);
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
    QCPMarginGroup* marginGroup;
    inline static int count;
    int savedXPos = 0;
    bool insertRight = false;
    QMap<QCPGraph*, QSharedPointer<SavedGraphColor>> savedGraphs = {};
    LogViewerItems::KeyType keyType = LogViewerItems::DateTime;
    QCPAxisRect *prepareRect();
    bool setPlot();
    void completeLastRect(QCPAxisRect*);
    void finishContainerRect(LogItem*);
    void makeMeasurementDlg(const QPoint &pos);
};
#endif //PROTOSLOGVIEWER_LOGVIEWER_H
