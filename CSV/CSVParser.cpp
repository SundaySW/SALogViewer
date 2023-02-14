//
// Created by 79162 on 11.02.2023.
//

#include "CSVParser.h"

CSVParser::CSVParser(QString &incFileName)
    :fileName(incFileName)
{
    File = new QFile(fileName);
    File->open(QIODevice::ReadWrite);
    fileDataStream = new QDataStream(File);
}

void CSVParser::prepareColumnNames() {
    fileDataStream->device()->seek(0);
    File->seek(0);
    QByteArray line = File->readLine();
    for (const auto& name : line.split(','))
        columns.append(name);
}

QVariantList CSVParser::makeNextBatchOfData(){
    QVariantList retVal;
    QByteArray line = File->readLine();
    for(const auto& value : line.split(','))
        retVal.append(value);
    return retVal;
}