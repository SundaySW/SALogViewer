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
    QVector<QVariant> makeNextBatchOfData();
    QSet<QString> getColumns();
    int getColumnIndex(const QString& columnName);
    virtual ~CSVParser();
private:
    char kSeparator_ = ',';
    QString fileName;
    std::unique_ptr<QFile> File;
    QDataStream* dataStream;
    QByteArray data;
    QVector<QString> columns;
    void prepareColumnNames();

    void resetColumns(const QList<QByteArray> &line);
};


#endif //SALOGVIEWER_CSVPARSER_H
