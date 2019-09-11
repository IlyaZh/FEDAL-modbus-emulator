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

            QJsonParseError *errorPtr;
            QJsonDocument jDoc = QJsonDocument::fromJson(fileData, errorPtr);
            if(errorPtr != nullptr) {
                sErrorString = errorPtr->errorString();
                emit errorOccurred();
            }
            QJsonObject jObj = jDoc.object();
            QJsonArray jArray = jObj.value("Commands").toArray();
            QString devName = jObj.value("Device").toString("No name");
            qDebug() << jDoc.isNull() << jDoc.isEmpty() << jDoc.isArray() << jDoc.isObject();
            foreach(QJsonValue item, jArray) {
                qDebug() << item.toVariant();
            }
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
