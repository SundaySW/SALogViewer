//
// Created by AlexKDR on 23.10.2022.
//

#include "PSQL_Driver.h"
#include "QSqlQuery"
#include "QSqlError"

#define Serial_Column_name "n"
#define DateTime_Column_name "datetime"

PSQL_Driver::PSQL_Driver(QJsonObject& conf)
        :config(conf)
        ,inError(false)
        ,connected(false)
{
    configUpdate();
}

PSQL_Driver::PSQL_Driver(PSQL_Driver* otherDriver):
    config(otherDriver->config)
    ,host(otherDriver->host)
    ,dbName(otherDriver->dbName)
    ,userName(otherDriver->userName)
    ,password(otherDriver->password)
    ,port(otherDriver->port)
{
    errorInDBDriver = otherDriver->errorInDBDriver;
    eventInDBDriver = otherDriver->eventInDBDriver;
}

bool PSQL_Driver::setConnection(const QString& connectionName){
    bool result;
    db = QSqlDatabase::addDatabase("QPSQL", connectionName.toLatin1());
    configUpdate();
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setPort(port.toInt());
    result = db.open(userName, password);
    if(result){
        connected = true;
        inError = false;
    }
    else {
        connected = false;
        inError = true;
        throwErrorToLog(QString("cant open database name: %1").arg(dbName));
    }
    return result;
}

bool PSQL_Driver::setConnection(){
    bool result;
    db = QSqlDatabase::addDatabase("QPSQL");
    configUpdate();
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(password);
    db.setPort(port.toInt());
    result = db.open();
    if(result){
        connected = true;
        inError = false;
        throwEventToLog(QString("opened database name: %1").arg(dbName));
        loadTableNames();
    }
    else {
        connected = false;
        inError = true;
        throwErrorToLog(QString("cant open database name: %1").arg(dbName));
    }
    return result;
}

void PSQL_Driver::loadTableNames(){
    QString queryStr = QString("SELECT table_name FROM information_schema.tables WHERE table_schema = 'public';");
    QSqlQuery query;
    if(!execMyQuery(query, queryStr)) {
        throwErrorToLog("while check table: " + query.lastError().text());
        return;
    }
    tableNames.clear();
    while (query.next())
        tableNames.insert(query.value(0).toString());
}

void PSQL_Driver::loadItemsInfo(){
    tablesInfo.clear();
    QVector<QString> info{};
    info.reserve(3);
    QSqlQuery query;
    for(const auto& tableName: tableNames){
        info.clear();
        if(!execMyQuery(query, QString(
                "(SELECT %2,%3 FROM %1 ORDER BY %2 DESC LIMIT 1)"
                "UNION"
                "(SELECT %2,%3 FROM %1 LIMIT 1)"
                "ORDER BY %2")
                .arg(tableName,
                     Serial_Column_name,
                     DateTime_Column_name
                     ))) return;
        query.next();
        info.append(query.value(1).toDateTime().toString("dd.MM.yyyy\nhh:mm:ss"));
        query.next();
        info.append(query.value(1).toDateTime().toString("dd.MM.yyyy\nhh:mm:ss"));
        info.append(query.value(0).toString());
        tablesInfo.insert(tableName, info);
    }
}

void PSQL_Driver::getLogItemData(const QString& tableName, const QString& keyN, const QString& valueN,
                                                QSqlQuery& query,
                                                const QString &dateFrom, const QString &dateTo,
                                                int rowFrom, int rowTo){
    QString queryStr = QString(
            "SELECT %1,%2 "
            "FROM %3 "
            "WHERE %4 BETWEEN %5 and %6 AND %1 BETWEEN '%7' AND '%8' "
            "ORDER by %4")
            .arg(
                    keyN,
                    valueN,
                    tableName,
                    Serial_Column_name,
                    QString::number(rowFrom),
                    QString::number(rowTo),
                    dateFrom,
                    dateTo);
    if(!execMyQuery(query, queryStr))
        throwErrorToLog("while loading data: " + query.lastError().text());
}

bool PSQL_Driver::hasTable(const QString& tableName){
    return tableNames.contains(tableName);
/*    QString queryStr = QString(
            "SELECT EXISTS (SELECT FROM information_schema.tables "
            "WHERE table_schema LIKE 'public' "
            "AND table_type LIKE 'BASE TABLE' "
            "AND table_name = '%1');").arg(tableName);
    QSqlQuery query;
    if(!execMyQuery(query, queryStr))
        throwErrorToLog("while check table: " + query.lastError().text());
    bool retVal = false;
    if(query.next()) retVal = query.value(0).toBool();
    return retVal;*/
}

bool PSQL_Driver::sendReq(const QString &queryStr){
    if(inError) return false;
    QSqlQuery query;
    if(!execMyQuery(query,queryStr))
    {
        inError = true;
        throwErrorToLog("while sending query: " + query.lastError().text());
        return false;
    }else
        return true;
}

bool PSQL_Driver::execMyQuery(QSqlQuery& query, const QString& queryStr){
    if(!connected)
        setConnection();
    query = db.exec(queryStr);
    if(db.lastError().isValid()){
        inError = true;
        throwErrorToLog("while sending query: " + query.lastError().text());
        return false;
    }
    return true;
}

bool PSQL_Driver::isDBOk() const {
    return connected && !inError;
}

QString PSQL_Driver::getTableInsertVars() {
    return {"DateTime, Event"};
}

QString PSQL_Driver::getTableInsertValues(const QString &eventStr) {
    return QString("current_timestamp, '%1'").arg(eventStr);
}

void PSQL_Driver::configUpdate() {
    auto confObject = config.value("DBConfObject");
    host = confObject["HostName"].toString();
    dbName = confObject["DatabaseName"].toString();
    userName = confObject["UserName"].toString();
    password = confObject["Password"].toString();
    port = confObject["Port"].toString();
    autoConnect = confObject["autoconnect"].toBool();
}

void PSQL_Driver::closeConnection() {
    connected = false;
    db.close();
    db.removeDatabase(QSqlDatabase::defaultConnection);
    throwEventToLog(QString("closed database name: %1").arg(dbName));
}

void PSQL_Driver::throwEventToLog(const QString& str){
    eventInDBDriver("Event in DB: " + str);
    //todo make signals
}

void PSQL_Driver::throwErrorToLog(const QString& str){
    errorInDBDriver("Error in DB: " + str);
    //todo make signals
}

bool PSQL_Driver::isAutoConnect() const{
    return autoConnect;
}

QSet<QString>& PSQL_Driver::getTableNames(){
    loadTableNames();
    return tableNames;
}

QMap<QString, QVector<QString>>& PSQL_Driver::getItemsInfo(){
    loadItemsInfo();
    return tablesInfo;
}

int PSQL_Driver::countOfRowsInTable(const QString& tableName){
    if(tablesInfo.contains(tableName))
        return getItemsInfo().value(tableName).at(2).toInt();
    else return 0;
}

void PSQL_Driver::loadAllItemsInfo(){
    loadTableNames();
    loadItemsInfo();
}

PSQL_Driver::~PSQL_Driver() {
//    closeConnection();
}

