//
// Created by 79162 on 11.02.2023.
//

#ifndef SALOGVIEWER_CSVPARSER_H
#define SALOGVIEWER_CSVPARSER_H


#include <QtCore/QString>
#include <QtCore/QFile>
#include <memory>
#include "QDataStream"
#include "QVariant"
#include "QSet"
#include "QByteArray"

class CSVParser {
public:
    explicit CSVParser(QString& fileName);
    void prepareColumnNames();
    QVector<QVariant> makeNextBatchOfData();
    QSet<QString> getColumns();
    int getColumnIndex(const QString& columnName);
    virtual ~CSVParser();
private:
    QString fileName;
    std::unique_ptr<QFile> File;
    QDataStream* dataStream;
    QByteArray data;
    QVector<QString> columns;
};


#endif //SALOGVIEWER_CSVPARSER_H
