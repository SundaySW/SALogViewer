//
// Created by 79162 on 11.02.2023.
//

#ifndef SALOGVIEWER_CSVPARSER_H
#define SALOGVIEWER_CSVPARSER_H


#include <QtCore/QString>
#include <QtCore/QFile>
#include "QDataStream"
#include "QVariant"

class CSVParser {
    explicit CSVParser(QString& fileName);
    void prepareColumnNames();
    QVariantList makeNextBatchOfData();
private:
    QString fileName;
    QFile* File;
    QDataStream* fileDataStream;
    QStringList columns;
};


#endif //SALOGVIEWER_CSVPARSER_H
