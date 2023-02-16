//
// Created by outlaw on 01.11.2022.
//
#include "Settings_dlg.h"

Settings_dlg::Settings_dlg(LogViewer* logViewer, QJsonObject& cfg, QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    setWindowTitle(QString("Settings"));
    auto* mainLayout = new QVBoxLayout(this);
    auto pDbConfigWidget = new dbsettingswdgt(cfg, logViewer->getDbDriver(), logViewer, this);
    mainLayout->addWidget(pDbConfigWidget);
    this->setLayout(mainLayout);
    connect(pDbConfigWidget, &dbsettingswdgt::eventInDBConnection, [this](){emit eventInSettings("", false);});
}
void Settings_dlg::closeEvent(QCloseEvent *event)
{
    close();
}