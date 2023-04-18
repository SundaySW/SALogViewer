#include "tooltipframe.h"
#include "ui_tooltipframe.h"

ToolTipFrame::ToolTipFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ToolTipFrame)
{
    ui->setupUi(this);
    list = ui->itemList;
}

ToolTipFrame::~ToolTipFrame()
{
    delete ui;
}

void ToolTipFrame::resetData(const QVector<LogViewerItems::ToolTipData> &newData, const QString &keyValue) {
    list->clear();
    int newWidth = 0;
    list->addItem(keyValue);
    for(const auto& data: newData){
        QString newString = QString("%1 : ").arg(data.value) + data.label;
        list->addItem(newString);
        list->item(list->count()-1)->setForeground(QBrush(data.color));
        if(newString.length() > newWidth)
            newWidth = newString.length();
    }
    this->setFixedSize(newWidth * 7.5, list->count() * 22);
    //todo find another way
}

