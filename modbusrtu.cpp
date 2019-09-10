#include "modbusrtu.h"

ModBusRtu::ModBusRtu(QString comPort, qint32 comBaud, quint8 address, QObject *parent) : QSerialPort(parent)
{
    this->setPortName(comPort);
    this->setBaudRate(comBaud);
    iAddress = address;
    errorMsg = "";

    connect(this, &QSerialPort::readyRead, this, &ModBusRtu::receiveHandler);
    connect(this, QSerialPort::errorOccurred(QSerialPort::SerialPortError error), this, serialErrorSlot(QSerialPort::SerialPortError error));

}

QMap<quint16, quint16> ModBusRtu::getChanges() {
    QMap<quint16, quint16> changes;
    if(!changedData.isEmpty()) {
        changes = changedData;
    }
    changedData.clear();
    return changes;
}

quint8 ModBusRtu::getAddress() {
    return iAddress;
}

QString ModBusRtu::errorString() {
    if(errorMsg.isEmpty()) {
        QString serialError = QSerialPort::errorString();
        if(!serialError.isEmpty()) {
           return serialError;
        }
    }
    return errorMsg;
}

// private slot
void ModBusRtu::receiveHandler() {
    quint8 addr;
    quint16 reg;
    quint8 comm;
    quint16 value;

    QByteArray package = readAll();
    addr = package.at(0);
    comm = package.at(1);
    reg = (package.at(2)<<8) | package.at(3);
    if(addr != iAddress) {
        errorMsg = QString("Wrong address. My address is %1, but received %2.").arg(iAddress).arg(addr);
        emit errorOccurred();
        return;
    }
    quint16 crc = CRC16(package);
    if(crc != 0) {
        errorMsg = QString("Wrong CRC \"%1\".").arg(crc);
        emit errorOccurred();
        return;
    }

    switch(comm) {
    case READ:
        break;
    case WRITE:
        if(package.length() == 8) {
            value = (package.at(4) << 8) | package.at(5);
        } else {
            errorMsg = "Modbus receive: Package length is not equals 8!";
            emit errorOccurred();
        }

        break;
    }


}

quint16 ModBusRtu::CRC16(QByteArray &p)
{
        if(p.length() > 255) return 0;

        quint8 crc_hi;
        quint8 crc_lo;

        crc_hi = 0xFF;   // high byte of CRC initialized
        crc_lo = 0xFF;   // low byte of CRC initialized

        foreach(char item, p) {
            quint8 i = crc_hi ^ item;        // will index into CRC lookup table
            crc_hi = crc_lo ^ (auchCRCHi[i]);    // calculate the CRC
            crc_lo =          (auchCRCLo[i]);
        }


        return ((crc_hi << 8) | crc_lo);
}
