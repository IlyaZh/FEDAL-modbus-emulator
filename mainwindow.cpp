#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    modbus(nullptr)
{
    ui->setupUi(this);

    updatePorts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// private slots
void MainWindow::updatePorts() {
    QStringList ports, bauds;
    ui->comBaudBox->clear();
    ui->comPortBox->clear();

    foreach(qint32 item, QSerialPortInfo::standardBaudRates()) {
        bauds.append(QString::number(item));
    }

    foreach(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        ports.append(port.portName());
    }

    ui->comPortBox->addItems(ports);
    ui->comBaudBox->addItems(bauds);
}

void MainWindow::on_comRefreshButton_clicked()
{
    updatePorts();
}

void MainWindow::on_comConnectButton_clicked(bool checked)
{

}
