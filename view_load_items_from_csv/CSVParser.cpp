//
// Created by 79162 on 11.02.2023.
//

#include "CSVParser.h"
#include <memory>

CSVParser::CSVParser(QString &incFileName)
    :fileName(incFileName)
{
    File = std::make_unique<QFile>(fileName);
    File->open(QIODevice::ReadOnly);
    data = File->readAll();
    dataStream = new QDataStream(data);
    File->close();
    prepareColumnNames();
}

void CSVParser::prepareColumnNames(){
    QByteArray line = dataStream->device()->readLine();
    columns.clear();
    for(const auto& name : line.split(','))
        columns.append(name);
}

QVector<QVariant> CSVParser::makeNextBatchOfData(){
    QVector<QVariant> retVal{};
    QByteArray line = dataStream->device()->readLine();
    if(line.isEmpty()) return retVal;
    for(const auto& value : line.split(','))
        retVal.append(value);
    return retVal;
}

QSet<QString> CSVParser::getColumns(){
    return QSet<QString>(columns.begin(), columns.end());
}

int CSVParser::getColumnIndex(const QString &columnName) {
    return columns.indexOf(columnName);
}

CSVParser::~CSVParser() {

}