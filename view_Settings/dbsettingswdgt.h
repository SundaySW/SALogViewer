#ifndef DBSETTINGSWDGT_H
#define DBSETTINGSWDGT_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <DB_Driver/PSQL_Driver.h>
#include <LogViewer.h>
#include "QJsonObject"
#include "QIcon"

QT_BEGIN_NAMESPACE
    namespace Ui {class dbsettingswdgt;}
QT_END_NAMESPACE

class dbsettingswdgt : public QWidget
{
    Q_OBJECT
public:
    explicit dbsettingswdgt(QJsonObject&, QSharedPointer<PSQL_Driver>, LogViewer*, QWidget* = nullptr);
    ~dbsettingswdgt();
    void Save();
    void Set();
signals:
    void eventInDBConnection(const QString&, bool);
private slots:
    void keyTypeChanged(int idx);
private:
    QIcon connectIcon;
    QIcon disconnectIcon;
    QJsonObject& savedConf;
    LogViewer* logViewer;
    QSharedPointer<PSQL_Driver> dbDriver;
    void setEditsStateDisabled(bool state);
    void updateView();
    Ui::dbsettingswdgt *ui;
};

#endif // DBSETTINGSWDGT_H
