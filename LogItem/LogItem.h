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
#include "QColor"

class LogItem: public TreeItem{
public:
    LogItem(const QString& tableName, TreeItem *parentItem = nullptr);
    const QString &getName() const;
    QBrush* getBrush();
    QColor getColor();
    bool setData(int column, const QVariant &value);

private:
    QString name;
    QString tableName;
    QVariantList xData;
    QVariantList yData;
    QColor itemColor;
    QColor generateColor();
};

#endif //PROTOSLOGVIEWER_LOGITEM_H
