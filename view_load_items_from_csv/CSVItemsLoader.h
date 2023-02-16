#ifndef LOAD_ITEMS_FROM_CSV_H
#define LOAD_ITEMS_FROM_CSV_H

#include <QWidget>
#include <LogViewer.h>
#include "CSVParser.h"
#include <view_treeview/treeModel.h>

namespace Ui {
class CSVItemsLoader;
}

class CSVItemsLoader : public QWidget
{
    Q_OBJECT
public:
    explicit CSVItemsLoader(LogItem* root, LogViewer* logViewer, QWidget *parent = nullptr);
    ~CSVItemsLoader();
    void loadNamesFromCSV();
    void reFresh();
signals:
    QVector<QString> needToResetModel();
private slots:
    void keyTypeChanged(int idx);
private:
    LogViewer* logViewer;
    TreeModel* csvTreeModel;
    QString fileName;
    std::unique_ptr<CSVParser> csvParser;
    QString keyAxisName{};
    std::optional<int> keyColumnIndex{};
    LogItem* csvRootItem;
    LogItem* mainRootItem;
    Ui::CSVItemsLoader *ui;
    void reFreshView();
    void setKeyAxisItem(LogItem *item);
    void loadDataToItems();
};

#endif // LOAD_ITEMS_FROM_CSV_H
