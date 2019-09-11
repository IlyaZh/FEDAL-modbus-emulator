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

            QJsonParseError *errorPtr = nullptr;
            QJsonDocument jDoc = QJsonDocument::fromJson(fileData, errorPtr);
            if(errorPtr != nullptr) {
                sErrorString = errorPtr->errorString();
                emit errorOccurred();
                return;
            }
            QJsonObject jObj = jDoc.object();
            QJsonArray jArray = jObj.value("Commands").toArray();
            devName = jObj.value("Device").toString("No name");
            foreach(QJsonValue item, jArray) {
                CommParse_t comm;
                QJsonArray itemArr = item.toArray();
                qDebug() << itemArr;
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
