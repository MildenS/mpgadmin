#ifndef HTTPWORKER_H
#define HTTPWORKER_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QStringList>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class HttpWorker : public QObject
{
    Q_OBJECT
public:
    explicit HttpWorker(QObject *parent = nullptr);

    QJsonDocument getExhibits();
    bool addExhibit(const QString& exhibitTitle, const QString& exhibitDescription,
                    const QString& mainImagePath, const QStringList& trainImagesPaths);
    bool deleteExhibit(const QString& id);

signals:

private:

    QNetworkAccessManager* network_manager;
};

#endif // HTTPWORKER_H
