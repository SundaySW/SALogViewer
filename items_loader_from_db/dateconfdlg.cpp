#include "dateconfdlg.h"
#include "ui_dateconfdlg.h"

DateConfDlg::DateConfDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateConfDlg)
{
    ui->setupUi(this);
    ui->RowCount->setVisible(false);
    ui->label_rows->setVisible(false);

    connect(ui->confirmBtn, &QPushButton::clicked, [this](){
        startDate = ui->startDate->text();
        endDate = ui->EndDate->text();
        count = ui->RowCount->text();
        hide();
        emit dateConfirmed(startDate,endDate,count);
    });
}

void DateConfDlg::loadRefs(const QString& startDate_, const QString& endDate_, const QString& count_){
    startDate = startDate_;
    endDate = endDate_;
    count = count_;
    ui->startDate->setText(startDate);
    ui->EndDate->setText(endDate);
    ui->RowCount->setText(count);
}

DateConfDlg::~DateConfDlg()
{
    delete ui;
}
