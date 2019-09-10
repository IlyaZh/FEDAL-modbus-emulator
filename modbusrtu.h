#ifndef MODBUSRTU_H
#define MODBUSRTU_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class ModBusRtu : public QSerialPort
{
    Q_OBJECT
public:
    explicit ModBusRtu(QString comPort, qint32 comBaud, QObject *parent = nullptr);

signals:


public slots:

private slots:

private:
};

#endif // MODBUSRTU_H
