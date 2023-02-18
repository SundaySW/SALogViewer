#ifndef TOOLTIPFRAME_H
#define TOOLTIPFRAME_H

#include <QFrame>
#include <main.h>
#include "QVector"

namespace Ui {
class ToolTipFrame;
}

class ToolTipFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ToolTipFrame(QWidget *parent = nullptr);
    ~ToolTipFrame();
    void resetData(const QVector<LogViewerItems::ToolTipData>& newData);
private:
    QListWidget* list;
    Ui::ToolTipFrame *ui;
};

#endif // TOOLTIPFRAME_H
