#include "sqltagmodel.h"

#include <QSqlRecord>

SqlTagModel::SqlTagModel(QObject* parent)
  : QSqlQueryModel(parent)
{}

QString
SqlTagModel::getId(const QModelIndex& index) const
{
    // 0: id
    const QString id = record(index.row()).value(0).toString();
    return id;
}

QString
SqlTagModel::getTag(const QModelIndex& index) const
{
    // 1: tag
    const QString tag = record(index.row()).value(1).toString();
    return tag;
}

Qt::ItemFlags
SqlTagModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QSqlQueryModel::flags(index);
    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | defaultFlags;
    }
    return defaultFlags;
}

QMimeData*
SqlTagModel::mimeData(const QModelIndexList& indexes) const
{
    if (indexes.isEmpty()) {
        return Q_NULLPTR;
    }
    QMimeData* data = new QMimeData();
    QByteArray ids;
    bool first = true;
    foreach (const QModelIndex& index, indexes) {
        if (index.column() != 0) {
            continue;
        }
        QString id = getId(index);
        if (!first) {
            ids += ";";
        } else {
            first = false;
        }
        ids += id;
    }
    data->setData("application/x-qabstractitemmodeldatalist", ids);
    return data;
}

QVariant
SqlTagModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        return getTag(index);
    }
    return QSqlQueryModel::data(index, role);
}
