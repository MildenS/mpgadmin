#include "httpworker.h"
#include <QUrlQuery>
#include <QErrorMessage>
#include <QEventLoop>
#include <QDebug>
#include <QHttpMultiPart>
#include <QMessageBox>
#include <QBuffer>
#include <QFileInfo>

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

       //qDebug() << QJsonDocument(databaseData);
       qDebug() << "Finish download database";
       return QJsonDocument(databaseData);
}

bool HttpWorker::addExhibit(const QString& exhibitTitle, const QString& exhibitDescription,
                            const QString& mainImagePath, const QStringList& trainImagesPaths)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // --- Title
    QHttpPart titlePart;
    titlePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"exhibit_title\""));
    titlePart.setBody(exhibitTitle.toUtf8());
    multiPart->append(titlePart);

    // --- Description
    QHttpPart descriptionPart;
    descriptionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"exhibit_description\""));
    descriptionPart.setBody(exhibitDescription.toUtf8());
    multiPart->append(descriptionPart);

    auto scaledImageData = [](const QString& imagePath) -> QByteArray {
        QImage image(imagePath);
        if (image.isNull()) {
            return {};
        }

        QSize maxSize(512, 512);
        QSize scaledSize = image.size().boundedTo(maxSize);
        QImage scaledImage = image.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        scaledImage.save(&buffer, "JPG", 90);
        return byteArray;
    };

    QByteArray mainImageBytes = scaledImageData(mainImagePath);
    if (mainImageBytes.isEmpty()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Cannot load or scale main image."));
        multiPart->deleteLater();
        return false;
    }
    QHttpPart mainImagePart;
    mainImagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant("form-data; name=\"main-image\"; filename=\"" + QFileInfo(mainImagePath).fileName() + "\""));
    mainImagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    mainImagePart.setBody(mainImageBytes);
    multiPart->append(mainImagePart);

    // --- Train images
    for (int i = 0; i < trainImagesPaths.size(); ++i) {
        QString path = trainImagesPaths[i];
        QByteArray imageBytes = scaledImageData(path);
        if (imageBytes.isEmpty()) {
            QMessageBox::critical(nullptr, tr("Error"), tr("Cannot load or scale training image: ") + path);
            multiPart->deleteLater();
            return false;
        }

        QHttpPart trainImagePart;
        QString key = QString("image-%1").arg(i);
        trainImagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                                 QVariant("form-data; name=\"" + key + "\"; filename=\"" + QFileInfo(path).fileName() + "\""));
        trainImagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
        trainImagePart.setBody(imageBytes);
        multiPart->append(trainImagePart);
    }

    QNetworkRequest request(QUrl("http://localhost:8888/add-exhibit"));
    QNetworkReply *reply = network_manager->post(request, multiPart);
    multiPart->setParent(reply);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QMessageBox::information(nullptr, tr("Success"), tr("Exhibit added successfully!"));
    } else {
        QMessageBox::critical(nullptr, tr("Error"), tr("Failed to send exhibit: ") + reply->errorString());
    }
    reply->deleteLater();
    return true;
}

bool HttpWorker::deleteExhibit(const QString &id)
{
    QUrl url("http://localhost:8888/delete-exhibit");
    QUrlQuery query;
    query.addQueryItem("exhibit-id", id);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = network_manager->deleteResource(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    bool success = false;
    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
        success = true;
    } else {
        qWarning() << "Failed to delete exhibit:" << reply->errorString();
    }

    reply->deleteLater();
    return success;
}
