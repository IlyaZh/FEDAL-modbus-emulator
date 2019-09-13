#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pModbus(nullptr)
{
    ui->setupUi(this);

    setupWindow();
}

MainWindow::~MainWindow() {

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        writeSettings();
        qInfo() << "App is closed";
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::setupWindow() {
    this->setWindowTitle(WINDOW_TITLE);
    setWindowIcon(QIcon(":/FEDAL.ico"));

    portIsOpen = false;
    minLimitsIsLoaded = maxLimitsIsLoaded = false;
    currentCommandId = 0;
    link = false;

    settings = new AppSettings(this);
    settingsDialog = new SettingsDialog(this);
    pDataParser = nullptr;
    pModbus = ;

    readSettings();

    bench = new DeviceForm(this);
    QGridLayout *benchLayout = new QGridLayout(ui->benchWidget);
    ui->benchWidget->setLayout(benchLayout);
    benchLayout->addWidget(bench);

    loadComData();

    setupConnections();

    if(settings->getPortAutoconnect()) {
        on_connectButton_clicked();
    }
}

void MainWindow::setupConnections() {
    // Применение новых настроек
    connect(settingsDialog, SIGNAL(accepted()), this, SLOT(settingsChanged()));

    // Запись измененного параметра в драйвер
    connect(bench, SIGNAL(updateParameter(quint16, quint16)), this, SLOT(prepareWrite(quint16, quint16)));
}

void MainWindow::loadComData() {
    ui->comAddressLabel->setText("Адрес устройства: " + QString::number(settings->getAddress()));
    QString comStateStr = QString("%1    %2 bps").arg(settings->getPortName()).arg(settings->getBaudRate());
    ui->comStateLabel->setText(comStateStr);
}

// private

void MainWindow::readSettings() {
    QPoint point = settings->getWindowPosition();
    QSize size = settings->getWindowSize();

    if(point != QPoint(-1, -1) && size != QSize(-1, -1)) {
        resize(size);
        move(point);
    }
}

void MainWindow::writeSettings() {
    settings->setWindowPosision(pos());
    settings->setWindowSize(size());
}

bool MainWindow::maybeSave() {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, APP_TITLE,
                               tr("Вы действительно хотите выйти?"),
                               QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
        return true;
    else
        return false;
}

// slots
void MainWindow::settingsChanged() {
    if(pModbus != nullptr) {
        if(pModbus->isOpen()) {
            pModbus->setOpenState(false);
            pModbus->setPort(settings->getPortName());
            pModbus->setBaud(settings->getBaudRate());
            pModbus->setAddress(settings->getAddress());
            pModbus->setOpenState(true);
        }
    }
    loadComData();
    qDebug() << "settings changed";
}

void MainWindow::onStateChanged(bool flag) {
    if(flag) {
        portIsOpen = true;
        minLimitsIsLoaded = maxLimitsIsLoaded = false;
        buttonStateChanged(true);
        qInfo() << "Port is open: " << settings->getPortName() + QString("   ") + QString::number(settings->getBaudRate()) + " bps";
        requestNextParam();
    } else {
        portIsOpen = false;
        buttonStateChanged(false);
        qInfo() << "Port is closed";
    }

    bench->setWidgetEnable(portIsOpen);
}

void MainWindow::buttonStateChanged(bool flag) {
    ui->connectButton->setChecked(flag);
    if(flag) {
        ui->connectButton->setText(settings->getPortName() + QString("   ") + QString::number(settings->getBaudRate()) + " bps");
    } else {
        ui->connectButton->setText("Подключить COM-порт");
    }
}

void MainWindow::readReady(bool queueIsEmpty) {
    quint16 startRegister = pModbus->getRegister();
    if(startRegister >= readRegisters[SerialPortHandler::MIN_REGS_SHIFT] && startRegister < readRegisters[SerialPortHandler::MIN_REGS_SHIFT]+countRegisters[SerialPortHandler::MIN_REGS_SHIFT])
        minLimitsIsLoaded = true;
    else if (startRegister >= readRegisters[SerialPortHandler::MAX_REGS_SHIFT] && startRegister < readRegisters[SerialPortHandler::MAX_REGS_SHIFT]+countRegisters[SerialPortHandler::MAX_REGS_SHIFT])
        maxLimitsIsLoaded = true;

    for(quint8 i = 0; i < pModbus->getCount(); i++) {
        quint16 item = pModbus->getValues().at(i);
        setDevParam(startRegister+i, item);
    }

    pModbus->clearBuffer();

    if (queueIsEmpty) requestNextParam();
}

void MainWindow::setDevParam(quint16 reg, quint16 value) {
    bench->setLink(true);

    switch (reg) {
    case DeviceForm::FREQUENCY_SHIFT:
        bench->setFrequency(value/pow(10, FREQUENCY_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::DURATION_SHIFT:
        bench->setDuration(value);
        break;
    case DeviceForm::CURRENT_SHIFT:
        bench->setCurrent(value/pow(10, CURRENT_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::VOLTAGE_SHIFT:
        bench->setVoltage(value/pow(10, VOLTAGE_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::DELAY_SHIFT:
        bench->setDelay(value);
        break;
    case DeviceForm::MIN_FREQUENCY_SHIFT:
        bench->setMinFrequency(value/pow(10, FREQUENCY_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::MAX_FREQUENCY_SHIFT:
        bench->setMaxFrequency(value/pow(10, FREQUENCY_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::MIN_DURATION_SHIFT:
        bench->setMinDuration(value);
        break;
    case DeviceForm::MAX_DURATION_SHIFT:
        bench->setMaxDuration(value);
        break;
    case DeviceForm::MIN_CURRENT_SHIFT:
        bench->setMinCurrent(value/pow(10, CURRENT_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::MAX_CURRENT_SHIFT:
        bench->setMaxCurrent(value/pow(10, CURRENT_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::MIN_VOLTAGE_SHIFT:
        bench->setMinVoltage(value/pow(10, VOLTAGE_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::MAX_VOLTAGE_SHIFT:
        bench->setMaxVoltage(value/pow(10, VOLTAGE_FRACTIAL_SYMBOLS));
        break;
    case DeviceForm::MIN_DELAY_SHIFT:
        bench->setMinDelay(value);
        break;
    case DeviceForm::MAX_DELAY_SHIFT:
        bench->setMaxDelay(value);
        break;
    case DeviceForm::BLOCKS_SHIFT:
        bench->setDeviceBlocks(value);
        break;
    case DeviceForm::STATUS_READ_SHIFT:
        bench->setDeviceState(value);
        break;
    default:
        break;
    }
}

void MainWindow::onTimeout(quint8) {
    bench->setLink(false);
    errorMessage("Timeout");

    minLimitsIsLoaded = maxLimitsIsLoaded = false;

    requestNextParam();
}

void MainWindow::requestNextParam() {
    if(!portIsOpen) return;

    if(maxLimitsIsLoaded == false) {
        currentCommandId = SerialPortHandler::MAX_REGS_SHIFT;
    }

    if(minLimitsIsLoaded == false) {
        currentCommandId = SerialPortHandler::MIN_REGS_SHIFT;
    }

    if (minLimitsIsLoaded && maxLimitsIsLoaded) {
        currentCommandId = SerialPortHandler::VALUES_REGS_SHIFT;
    }

    if(pModbus->queueIsEmpty()) {
        pModbus->dataToRead(readRegisters[currentCommandId], countRegisters[currentCommandId]);
    }

}

void MainWindow::errorMessage(QString msg) {
    link = false;
    ui->statusBar->showMessage(msg, STATUSBAR_MESSAGE_TIMEOUT);
    qInfo() << "Error message: " << msg;
}

void MainWindow::on_connectButton_clicked()
{
    loadComData();

    QString port = settings->getPortName();
    if(port.isEmpty()) return;

    if(pModbus == nullptr) {
        pModbus = new SerialPortHandler(this);
        connect(pModbus, SIGNAL(stateChanged(bool)), this, SLOT(onStateChanged(bool)));
        connect(pModbus, &SerialPortHandler::errorOccuredSignal, [this]() {
            errorMessage(pModbus->errorString());
        });
        connect(pModbus, SIGNAL(timeoutSignal(quint8)), this, SLOT(onTimeout(quint8)));
        connect(pModbus, SIGNAL(newDataIsReady(bool)), this, SLOT(readReady(bool)));
    }

    if(pModbus->isOpen()) {
        pModbus->setOpenState(false);
        ui->connectButton->setChecked(false);
    } else {
        pModbus->setBaud(settings->getBaudRate());
        pModbus->setPort(settings->getPortName());
        pModbus->setAddress(settings->getAddress());
        pModbus->setTimeout(COM_TIMEOUT);
        pModbus->setOpenState(true);
        if(!pModbus->isOpen()) {
            errorMessage("Подключение неудалось");
            ui->connectButton->setChecked(false);
        }
    }
}

void MainWindow::on_settingsButton_clicked()
{
    settingsDialog->updateParameters();
    settingsDialog->show();
}

void MainWindow::prepareWrite(quint16 reg, quint16 value) {
    pModbus->dataToWrite(reg, value);
}
