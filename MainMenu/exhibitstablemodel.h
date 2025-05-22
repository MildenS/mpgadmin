#ifndef EXHIBITSTABLEMODEL_H
#define EXHIBITSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QModelIndex>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>


class ExhibitsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ExhibitsTableModel(const QJsonDocument& exhibits, QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QJsonObject getExhibitInfo(const QModelIndex &index) const;

private:

    QJsonArray exhibits_internal;
    QStringList headers;

    // QAbstractItemModel interface
public:
    bool setData(const QModelIndex &index, const QVariant &value, int role);
};

#endif // EXHIBITSTABLEMODEL_H
