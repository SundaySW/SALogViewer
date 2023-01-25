#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <view_Settings/Settings_dlg.h>
#include <QtCore/QFile>
#include <view_treeview/treeModel.h>
#include <view_treeview/TreeItem.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void plotTest();
private:
    QJsonObject ConfJson;
    QFile* configFile;
    LogViewer* logViewer;
    TreeModel* treeModel;
    Settings_dlg* settingsDlg;
    Ui::MainWindow *ui;
    void newPlots();
    void openFileLoadConfig();
    void AddToLog(const QString &string, bool isError = false);
    void checkServicesConnection();
};
#endif // MAINWINDOW_H