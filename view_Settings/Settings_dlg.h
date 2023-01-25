//
// Created by outlaw on 01.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_SETTINGS_DLG_H
#define POTOSSERVER_PARAMSERVICE_SETTINGS_DLG_H

#include <QtWidgets/QDialog>
#include "QComboBox"
#include "QHBoxLayout"
#include "dbsettingswdgt.h"
#include <QtWidgets/QPushButton>
#include "QButtonGroup"
#include <QGroupBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSplitter>
#include <DB_Driver/PSQL_Driver.h>
#include "QJsonObject"
#include "LogViewer.h"

class Settings_dlg : public QDialog
{
    Q_OBJECT
public:
    Settings_dlg(LogViewer* logViewer, QJsonObject&, QWidget *parent = nullptr);
signals:
    void eventInSettings(const QString&, bool);
private:
    QTabWidget* tabWidget;
    void closeEvent(QCloseEvent *event) override;
};


#endif //POTOSSERVER_PARAMSERVICE_SETTINGS_DLG_H
