//
// Created by AlexKDR on 23.10.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_PSQL_DRIVER_H
#define POTOSSERVER_PARAMSERVICE_PSQL_DRIVER_H

#include <QSqlDatabase>
#include <QtCore/QJsonObject>
#include "functional"
#include <qcustomplot/qcustomplot.h>
#include "LogItem/LogItem.h"


#define Serial_Column_name "n"
#define DateTime_Column_name "DateTime"
#define N_of_columns 6
#define Serial_column 0
#define DateTime_column 1


class PSQL_Driver {
public:
    std::function<void(const QString&)> errorInDBDriver;
    std::function<void(const QString&)> eventInDBDriver;
    explicit PSQL_Driver(QJsonObject &conf);
    bool setConnection();
    bool isAutoConnect() const;
    void closeConnection();
    [[nodiscard]] bool isDBOk() const;
    QSet<QString>& getTableNames();
    QMap<QString, QVector<QString>> &getItemsInfo();
    void getLogItemData(const QString&, QVector<QVariant>&, const QString&);
private:
    QSqlDatabase db;
    QJsonObject& config;
    QString host, dbName, userName, password, port;
    QString eventsTableName = "events";
    QString viewChangesTName = "view_changes";
    QSet<QString> tableNames;
    QMap<QString, QVector<QString>> tablesInfo;
    bool inError;
    bool connected;
    bool autoConnect;
    void throwEventToLog(const QString &str);
    void throwErrorToLog(const QString &str);
    static QString getTableInsertVars();
    static QString getTableInsertValues(const QString &eventStr);
    void loadTableNames();
    void configUpdate();
    bool execMyQuery(QSqlQuery&, const QString&);
    bool hasTable(const QString &tableName);
    bool sendReq(const QString &queryStr);
    void loadItemsInfo();
};


#endif //POTOSSERVER_PARAMSERVICE_PSQL_DRIVER_H
