#include "httpworker.h"
#include <QUrlQuery>
#include <QErrorMessage>
#include <QEventLoop>
#include <QDebug>

HttpWorker::HttpWorker(QObject *parent) : QObject(parent)
{
    network_manager = new QNetworkAccessManager;
}

QJsonDocument HttpWorker::getExhibits()
{
    QJsonArray databaseData;
    QString nextChunkToken = "";
    bool isLastChunk = false;

       do {
           QUrl url("http://localhost:8888/get-database-chunk");
           QUrlQuery query;
           query.addQueryItem("next-chunk-token", nextChunkToken);
           url.setQuery(query);

           QNetworkRequest request(url);
           QNetworkReply* reply = network_manager->get(request);

           QEventLoop loop;
           connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
           loop.exec();

           if (reply->error() != QNetworkReply::NoError) {
               qWarning() << "Getting chunk error:" << reply->errorString();
               reply->deleteLater();
               break;
           }

           QByteArray responseData = reply->readAll();
           reply->deleteLater();

           QJsonParseError parseError;
           QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseError);

           if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
               qWarning() << "Parsing JSON error:" << parseError.errorString();
               break;
           }

           QJsonObject responseObj = jsonDoc.object();

           QJsonArray chunkArray = responseObj["exhibits"].toArray();
           for (const QJsonValue& val : chunkArray) {
               databaseData.append(val);
               qDebug() << "new chunk exhibit value: " << val["exhibit_title"].toString();
           }

           isLastChunk = responseObj["is_last"].toBool();
           nextChunkToken = responseObj["next_chunk"].toString();

       } while (!isLastChunk && !nextChunkToken.isEmpty());

       qDebug() << "Finish download database";
       return QJsonDocument(databaseData);
}
