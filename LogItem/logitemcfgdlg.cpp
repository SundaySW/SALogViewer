#include "logitemcfgdlg.h"
#include "ui_logitemcfgdlg.h"

LogItemCfgDlg::LogItemCfgDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogItemCfgDlg)
{
    ui->setupUi(this);
}

LogItemCfgDlg::~LogItemCfgDlg()
{
    delete ui;
}
