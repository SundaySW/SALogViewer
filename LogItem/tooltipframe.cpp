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

void ToolTipFrame::resetData(const QVector<LogViewerItems::ToolTipData>& newData) {
    list->clear();
    int newWidth = 0;
    for(const auto& data: newData){
        QString newString = data.label+QString(": %1").arg(data.value);
        list->addItem(newString);
        list->item(list->count()-1)->setForeground(QBrush(data.color));
        if(newString.length() > newWidth)
            newWidth = newString.length();
    }
    this->setFixedHeight(list->count()*24);
    this->setFixedWidth(newWidth*8);
    //todo find another way
}

