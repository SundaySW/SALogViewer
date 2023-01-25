//
// Created by 79162 on 07.01.2023.
//

#include "LogViewer.h"

LogViewer::LogViewer(QJsonObject &inJson):
    qJsonObject(inJson)
    ,dbDriver(PSQL_Driver(inJson))
{
    dbDriver.errorInDBDriver = [this](const QString& errorStr){ errorInDBToLog(errorStr);};
    dbDriver.eventInDBDriver = [this](const QString& eventStr){ eventInDBToLog(eventStr);};
    if(dbDriver.isAutoConnect()) dbDriver.setConnection();
}

PSQL_Driver& LogViewer::getDbDriver() {
    return dbDriver;
}

bool LogViewer::isDataBaseOk() {
    return dbDriver.isDBOk();
}

void LogViewer::fetchData(){
//    LogItem item1("1");
//    LogItem item2("2");
//    items.append(item1);
//    items.append(item2);
//    items[0].children.append(items[1]);
//    items[0].mapped = false;
//    items[1].parent = &items[0];
}

QVector<LogItem*>& LogViewer::getItemPtrList() {
    return items;
}