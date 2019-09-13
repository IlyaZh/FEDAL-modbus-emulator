#include "dataparser.h"

DataParser::DataParser(QString filename, QObject *parent) : QObject(parent)
{
    sFilename = filename;
    sErrorString = "";

    QFile *file = new QFile(sFilename);;

    if(file->exists()) {
        if(file->open(QIODevice::ReadOnly)) {
            QByteArray fileData = file->readAll();
            file->close();
            file->deleteLater();

            QJsonParseError *errorPtr = nullptr;
            QJsonDocument jDoc = QJsonDocument::fromJson(fileData, errorPtr);
            if(errorPtr != nullptr) {
                sErrorString = errorPtr->errorString();
                emit errorOccurred();
                return;
            }
            Values.clear();
            QJsonObject jObj = jDoc.object();
            QJsonArray commArr = jObj.value("Commands").toArray();
            foreach(QJsonValue item, commArr) {
                QJsonArray itemArr = item.toArray();
                quint16 iComm = static_cast<quint16>(itemArr.at(0).toString().toInt(nullptr, 16));
                quint16 iValue = static_cast<quint16>(itemArr.at(1).toInt());
                Values.insert(iComm, iValue);
            }
            emit parsingComplete();
        } else {
            sErrorString = file->errorString();
            emit errorOccurred();
        }
    } else {
        sErrorString = file->errorString();
        emit errorOccurred();
    }


}

QString DataParser::filename() {
    return sFilename;
}

QString DataParser::errorString() {
    return sErrorString;
}

QMap<quint16, double> DataParser::getValues() {
    return Values;
}
