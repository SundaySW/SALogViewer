//
// Created by user on 31.01.2023.
//

#ifndef PROTOSLOGVIEWER_MAIN_H
#define PROTOSLOGVIEWER_MAIN_H

#include <CustomTicker/MsecTicker.h>

#define LogVieverColor_white QRgb(0xBBBBBB)
#define LogVieverColor_dark QRgb(0x45494A)
#define LogVieverColor_middle QRgb(0x545657)
#define LogVieverColor_middle2 QRgb(0x7C7E80)
#define LogVieverColor_grid QRgb(0x616161)
#define LogVieverColor_subgrid QRgb(0x4F5152)
#define LogVieverColor_bottomTick QRgb(0xE3E6E8)
#define LogVieverColor_selectedGraph QRgb(0xbd8e3e)

namespace LogViewerItems{
    enum Type: int{
        Regular,
        Container,
        Root
    };
    enum KeyType: int{
        DateTime = 0,
        mSecs = 1,
        Count = 2
    };

    inline static double prepareKeyData(const QVariant& data, KeyType keyType){
        switch (keyType) {
            case DateTime:
                return QCPAxisTickerDateTime::dateTimeToKey(data.toDateTime().toUTC());
            case mSecs:
            case Count:
            default:
                return data.toDouble();
        }
    }
    inline static void bindTickerToAxis(QCPAxis* axis, KeyType keyType){
        if(keyType == DateTime){
            QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
            dateTimeTicker->setDateTimeFormat("MM.dd-hh:mm");
            axis->setTicker(dateTimeTicker);
        }
        else if(keyType == mSecs){
            QSharedPointer<MsecTicker> msecTicker(new MsecTicker);
            axis->setTicker(msecTicker);
        }else{

        }
    }

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
    struct ToolTipData{
        QColor color;
        QString label;
        double value;
    };
}

#endif //PROTOSLOGVIEWER_MAIN_H
