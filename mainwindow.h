#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.h"
#include "deviceform.h"
#include "appsettings.h"
#include "settingsdialog.h"
#include "modbusrtu.h"
#include "dataparser.h"

#include <QMainWindow>
#include <QSerialPort>
#include <QGridLayout>
#include <QCloseEvent>
#include <QMessageBox>

class SerialPortHandler;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent* event);

private:
    Ui::MainWindow *ui;
    DeviceForm* bench;
    AppSettings* settings;
    SettingsDialog* settingsDialog;
    ModBusRtu *pModbus;
    DataParser *pDataParser;
    void setupWindow();
    void loadComData();
    void setupConnections();
    void readSettings();
    void writeSettings();
    bool maybeSave();

public slots:
    void settingsChanged();

private slots:
    void onStateChanged(bool);
    void setDevParam(quint16, quint16);
    void onTimeout(quint8);
    void errorMessage(QString);
    void on_connectButton_clicked();
    void on_settingsButton_clicked();
    void buttonStateChanged(bool);
    void prepareWrite(quint16, quint16);
};

#endif // MAINWINDOW_H
