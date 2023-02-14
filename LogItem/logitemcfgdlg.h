#ifndef LOGITEMCFGDLG_H
#define LOGITEMCFGDLG_H

#include <QDialog>

namespace Ui {
class LogItemCfgDlg;
}

class LogItemCfgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LogItemCfgDlg(QWidget *parent = nullptr);
    ~LogItemCfgDlg();

private:
    Ui::LogItemCfgDlg *ui;
};

#endif // LOGITEMCFGDLG_H
