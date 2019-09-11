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

signals:
    void errorOccurred();

public slots:
private:
    QString sFilename;
    QString sErrorString;

};

#endif // DATAPARSER_H
