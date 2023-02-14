//
// Created by 79162 on 07.01.2023.
//

#ifndef PROTOSLOGVIEWER_LOGVIEWER_H
#define PROTOSLOGVIEWER_LOGVIEWER_H

#include <DB_Driver/PSQL_Driver.h>
#include <LogItem/LogItem.h>
#include <qcustomplot/qcustomplot.h>

#include <utility>
#include "QWidget"

class LogViewer: public QObject{
Q_OBJECT
public:
    struct SavedGraphColor{
        QPen savedPen;
        QCPGraph* savedGraph = nullptr;
        QCPItemText* text = nullptr;
        QCustomPlot* customPlot = nullptr;

        SavedGraphColor(QPen pen, QCPGraph* plottable, QCustomPlot* cPlot)
            :savedPen(std::move(pen)), savedGraph(plottable), customPlot(cPlot)
        {
            auto* decorator = new QCPSelectionDecorator();
            decorator->setPen(plottable->pen());
            auto alphaColor = plottable->pen().color();
            alphaColor.setAlpha(40);
            decorator->setBrush(QBrush(alphaColor));
            savedGraph->setSelectionDecorator(decorator);
            savedGraph->setPen(QPen(QColor(LogVieverColor_middle)));
        }
        void setText(){
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
                    text->position->setCoords(0.2, 0);
                }else
                    text->position->setCoords(0.5, 0.9);
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
        ~SavedGraphColor(){
            savedGraph->setPen(savedPen);
            customPlot->removeItem(text);
        }
    };

    explicit LogViewer(QJsonObject&, QCustomPlot*, LogItem* root, QWidget* _parent = nullptr);
    PSQL_Driver& getDbDriver();
    bool isDataBaseOk();
    void insertGraph(LogItem*, bool last = false);
    void removeGraph(LogItem*);
    void removeItem(LogItem*);
    void rebuildLayout();
    void loadItemFromDB(LogItem *item);
signals:
    void errorInDBToLog(const QString&);
    void eventInDBToLog(const QString&);
private slots:
    void plotContextMenuRequest(const QPoint& pos);
private:
    QWidget* mainWin;
    QJsonObject& qJsonObject;
    PSQL_Driver dbDriver;
    QCustomPlot* Plot;
    LogItem* rootItem;
    QCPMarginGroup* marginGroup;
    inline static int count;
    bool insertRight = false;
    QMap<QCPGraph*, SavedGraphColor*> savedGraphs = {};

    QCPAxisRect *prepareRect(bool last);
    bool setPlot();
    void completeLastRect(QCPAxisRect*);
    void finishContainerRect(LogItem*);
    void makeMeasurementDlg(const QPoint &pos);
};
#endif //PROTOSLOGVIEWER_LOGVIEWER_H
