#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>

#include <QDebug>


class DataParser : public QObject
{
    Q_OBJECT
public:
    explicit DataParser(QString filename, QObject *parent = nullptr);
    QString filename();
    QString errorString();
    QMap<quint16, double> getValues();



signals:
    void errorOccurred();
    void parsingComplete();

public slots:
private:
    QString sFilename;
    QString sErrorString;
    QMap<quint16, double> Values;


};

#endif // DATAPARSER_H
