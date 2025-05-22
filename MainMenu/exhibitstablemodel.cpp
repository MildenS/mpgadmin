#include "exhibitstablemodel.h"
#include <QJsonObject>
#include <QDebug>
#include <QPixmap>

ExhibitsTableModel::ExhibitsTableModel(const QJsonDocument& exhibits, QObject *parent):
    QAbstractTableModel(parent)
{
    if (!exhibits.isArray())
    {
        qDebug() << "ExhibitsTableModel error: invalid json";
    }
    exhibits_internal = exhibits.array();
    if (!exhibits_internal.isEmpty() && exhibits_internal[0].isObject())
        headers << "exhibit_title" <<  "exhibit_image" << "exhibit_id";
}


int ExhibitsTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return exhibits_internal.size();
}

int ExhibitsTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headers.size();
}

QVariant ExhibitsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};
    const QJsonObject obj = exhibits_internal[index.row()].toObject();
    QString key = headers[index.column()];

    if (role == Qt::DisplayRole && (key == "exhibit_title" || key == "exhibit_id")) {
        return obj.value(key).toString();
    }

    if (role == Qt::DecorationRole && key == "exhibit_image") {
        QByteArray base64 = obj.value(key).toString().toUtf8();
        QPixmap pixmap;
        pixmap.loadFromData(QByteArray::fromBase64(base64));
        return pixmap.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return {};
}

QJsonObject ExhibitsTableModel::getExhibitInfo(const QModelIndex &index) const
{
    return exhibits_internal[index.row()].toObject();
}


bool ExhibitsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return true;
}
