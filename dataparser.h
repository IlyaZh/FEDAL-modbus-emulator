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


    typedef struct {
        QString sName;
        QString sUnit;
        double dDivider;
        QPair<quint16, double> Value;
        QPair<quint16, double> Min;
        QPair<quint16, double> Max;
    } CommParse_t;
    QList<CommParse_t> getCommands();
    QMap<quint16, double> getValues();



signals:
    void errorOccurred();
    void parsingComplete();

public slots:
private:
    QString sFilename;
    QString sErrorString;
    QString devName;
    QMap<quint16, double> Values;
    QList<CommParse_t> Commands;


};

#endif // DATAPARSER_H
