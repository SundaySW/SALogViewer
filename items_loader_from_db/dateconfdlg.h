#ifndef DATECONFDLG_H
#define DATECONFDLG_H

#include <QDialog>

namespace Ui {
class DateConfDlg;
}

class DateConfDlg : public QDialog
{
    Q_OBJECT
public:
    explicit DateConfDlg(QWidget *parent = nullptr);
    ~DateConfDlg();
    void loadRefs(const QString&, const QString&, const QString&);
signals:
    void dateConfirmed(const QString&, const QString&, const QString&);
private:
    Ui::DateConfDlg *ui;
    QString startDate;
    QString endDate;
    QString count;
};

#endif // DATECONFDLG_H
