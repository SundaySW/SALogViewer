#ifndef ITEMSLOADER_H
#define ITEMSLOADER_H

#include <QWidget>
#include <LogViewer.h>
#include <treeModel.h>
#include "dateconfdlg.h"

namespace Ui {
class ItemsLoader;
}

class ItemsLoader : public QWidget
{
    Q_OBJECT
public:
    explicit ItemsLoader(LogItem* root, LogViewer* logViewer, QWidget *parent = nullptr);
    ~ItemsLoader();
    void loadNamesFromDB();
    void reFresh();
signals:
    QVector<QString> needToResetModel(const QString&);

private:
    LogItem* tableRootItem;
    LogItem* mainRootItem;
    TreeModel* tableTreeModel;
    LogViewer* logViewer;
    QSharedPointer<PSQL_Driver> driver;
    Ui::ItemsLoader *ui;
    DateConfDlg* dateConfDlg;
    QString startDate;
    QString endDate;
    QString rowCount;
    void loadDataToItem(LogItem *item, const QString &dateFrom, const QString &dateTo, const QString &count);
};

#endif // ITEMSLOADER_H