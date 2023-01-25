//
// Created by 79162 on 07.01.2023.
//

#include "LogItem.h"

extern QRandomGenerator generator;

LogItem::LogItem(const QString& _tableName, TreeItem *parentItem):
    TreeItem(QVector<QVariant>{_tableName}, parentItem)
    ,tableName(_tableName)
{
    itemColor = generateColor();
}

QColor LogItem::generateColor(){
    auto randColor = QColor(generator.bounded(255), generator.bounded(255), generator.bounded(255));
    float value = (randColor.red() + randColor.green() + randColor.blue())/3;
    float newValue = value + generator.bounded(50);
    float valueRatio = newValue / value;
    auto newColor = QColor();
    newColor.setRed(randColor.red() * valueRatio);
    newColor.setGreen(randColor.green() * valueRatio);
    newColor.setBlue(randColor.blue() * valueRatio);
    return newColor;
}

bool LogItem::setData(int column, const QVariant &value) {
    if(column == 0) name = value.toString();
    return TreeItem::setData(column, value);
}

QBrush* LogItem::getBrush() {
    return new QBrush(itemColor);
}

QColor LogItem::getColor(){
    return itemColor;
}

const QString &LogItem::getName() const {
    return tableName;
}
