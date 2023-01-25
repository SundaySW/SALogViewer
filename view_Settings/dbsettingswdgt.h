#ifndef DBSETTINGSWDGT_H
#define DBSETTINGSWDGT_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <DB_Driver/PSQL_Driver.h>
#include "QJsonObject"
#include "QIcon"

QT_BEGIN_NAMESPACE
    namespace Ui {class dbsettingswdgt;}
QT_END_NAMESPACE

class dbsettingswdgt : public QWidget
{
    Q_OBJECT
public:
    explicit dbsettingswdgt(QJsonObject &JsonConf, PSQL_Driver& databaseDriver, QWidget *parent = nullptr);
    ~dbsettingswdgt();
    void Save();
    void Set();
signals:
    void eventInDBConnection(const QString&, bool);
private:
    QIcon connectIcon;
    QIcon disconnectIcon;
    QJsonObject& savedConf;
    PSQL_Driver& dbDriver;
    void setEditsStateDisabled(bool state);
    void updateView();
    Ui::dbsettingswdgt *ui;
};

#endif // DBSETTINGSWDGT_H
