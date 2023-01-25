#include "dbsettingswdgt.h"
#include "ui_dbsettingswdgt.h"

dbsettingswdgt::dbsettingswdgt(QJsonObject &JsonConf, PSQL_Driver &databaseDriver, QWidget *parent):
    QWidget(parent)
    , savedConf(JsonConf)
    , dbDriver(databaseDriver)
    , ui(new Ui::dbsettingswdgt)
{
    ui->setupUi(this);
    connectIcon.addFile(QString::fromUtf8(":/icons/connect.svg"), QSize(35, 35), QIcon::Normal, QIcon::Off);
    disconnectIcon.addFile(QString::fromUtf8(":/icons/disconnect.svg"), QSize(35, 35), QIcon::Normal, QIcon::Off);
    Set();
    updateView();
    connect(ui->connectBtn, &QPushButton::clicked, [this](){
        if(dbDriver.isDBOk())
            dbDriver.closeConnection();
        else{
            Save();
            if(!dbDriver.setConnection()){
                ui->statusLabel->setText("Error on connect to DB: " + ui->DatabaseName->text());
                ui->statusLabel->setStyleSheet("color:red");
                return;
            }
        }
        emit eventInDBConnection("", false);
        updateView();
    });
    connect(ui->saveBtn, &QPushButton::clicked, [this](){Save();});
}

void dbsettingswdgt::Save() {
    QJsonObject confObj;
    confObj["HostName"] = ui->HostName->text();
    confObj["DatabaseName"] = ui->DatabaseName->text();
    confObj["UserName"] = ui->UserName->text();
    confObj["Password"] = ui->Password->text();
    confObj["Port"] = ui->Port->text();
    confObj["autoconnect"] = ui->autoconnect->isChecked();
    savedConf["DBConfObject"] = confObj;
}

void dbsettingswdgt::Set() {
    auto confObject = savedConf.value("DBConfObject");
    ui->HostName->setText((confObject["HostName"].toString()));
    ui->DatabaseName->setText (confObject["DatabaseName"].toString());
    ui->UserName->setText(confObject["UserName"].toString());
    ui->Password->setText  (confObject["Password"].toString());
    ui->Port->setText(confObject["Port"].toString());
    ui->autoconnect->setChecked(confObject["autoconnect"].toBool());
}

void dbsettingswdgt::setEditsStateDisabled(bool state) {
    ui->HostName->setDisabled(state);
    ui->DatabaseName->setDisabled(state);
    ui->UserName->setDisabled(state);
    ui->Password->setDisabled(state);
    ui->Port->setDisabled(state);
    ui->autoconnect->setDisabled(state);
    ui->saveBtn->setDisabled(state);
}

void dbsettingswdgt::updateView() {
    if(dbDriver.isDBOk()){
        ui->connectBtn->setIcon(disconnectIcon);
        setEditsStateDisabled(true);
        ui->statusLabel->setText("Connected to DB: " + ui->DatabaseName->text());
        ui->statusLabel->setStyleSheet("color:green");
    }else{
        setEditsStateDisabled(false);
        ui->connectBtn->setIcon(connectIcon);
        ui->statusLabel->setText("Disconnected from DB");
        ui->statusLabel->setStyleSheet("color:red");
    }
}

dbsettingswdgt::~dbsettingswdgt()
{
    delete ui;
}