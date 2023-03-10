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
    void getLogItemData(const QString&, QVector<QVariant>&, const QString&);
private:
    QSqlDatabase db;
    QJsonObject& config;
    QString host, dbName, userName, password, port;
    QString eventsTableName = "events";
    QString viewChangesTName = "view_changes";
    QSet<QString> tableNames;
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
};


#endif //POTOSSERVER_PARAMSERVICE_PSQL_DRIVER_H
