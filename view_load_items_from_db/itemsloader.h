#ifndef ITEMSLOADER_H
#define ITEMSLOADER_H

#include <QWidget>
#include <LogViewer.h>
#include <view_treeview/treeModel.h>

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
    QVector<QString> needToResetModel();
private:
    LogItem* tableRootItem;
    LogItem* mainRootItem;
    TreeModel* tableTreeModel;
    LogViewer* logViewer;
    PSQL_Driver& driver;
    Ui::ItemsLoader *ui;
};

#endif // ITEMSLOADER_H