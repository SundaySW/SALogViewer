#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <view_Settings/Settings_dlg.h>
#include <QtCore/QFile>
#include <view_treeview/treeModel.h>
#include <view_load_items_from_db/itemsloader.h>
#include <qcustomplot/qcustomplot.h>
#include <view_load_items_from_csv/CSVItemsLoader.h>
#include "LogItem/LogItem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void ContextMenuRequested(const QPoint &pos);
private:
    QJsonObject ConfJson;
    QFile* configFile;
    LogViewer* logViewer;
    LogItem* rootItem;
    LogItem* storedRoot;
    LogViewerItems::LastSource lastSource;
    ItemsLoader* DBitemsLoader;
    CSVItemsLoader* csvItemsLoader;
    TreeModel* treeModel;
    QCustomPlot* Plot;
    QDialog* loaderDBDlg;
    QDialog* loaderCSVDlg;
    Settings_dlg* settingsDlg;
    Ui::MainWindow *ui;

    void openFileLoadConfig();
    void AddToLog(const QString &string, bool isError = false);
    void checkServicesConnection();
    void setupModelData();
    void configDBLoaderDlg();
    void configCSVLoaderDlg();
    void setConnections();
    void configureUI();
    void closeEvent(QCloseEvent *event) override;
    void saveAll();
};
#endif // MAINWINDOW_H