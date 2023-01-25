//
// Created by 79162 on 07.01.2023.
//

#ifndef PROTOSLOGVIEWER_LOGVIEWER_H
#define PROTOSLOGVIEWER_LOGVIEWER_H

#include <DB_Driver/PSQL_Driver.h>
#include <LogItem/LogItem.h>
#include <view_treeview/ItemModel.h>

class LogViewer: public QObject{
Q_OBJECT
public:
    explicit LogViewer(QJsonObject &inJson);
    PSQL_Driver& getDbDriver();
    bool isDataBaseOk();
    QVector<LogItem*>& getItemPtrList();
signals:
    void needToResetModels();
    void errorInDBToLog(const QString&);
    void eventInDBToLog(const QString&);
private:
    QJsonObject& qJsonObject;
    QVector<LogItem*> items = {};
    PSQL_Driver dbDriver;
    void fetchData();
};


#endif //PROTOSLOGVIEWER_LOGVIEWER_H
