#include "modbusrtu.h"

ModBusRtu::ModBusRtu(QString comPort, qint32 comBaud, QObject *parent) : QSerialPort(parent)
{
    this->setPortName(comPort);
    this->setBaudRate(comBaud);

}
