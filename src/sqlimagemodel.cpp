#include "sqlimagemodel.h"

#include <QDesktopServices>
#include <QFile>
#include <QPixmap>
#include <QPixmapCache>
#include <QSqlRecord>
#include <QUrl>

SqlImageModel::SqlImageModel(DatabaseManager* dbMan,
                             const QSize& size,
                             QObject* parent)
  : QSqlQueryModel(parent)
{
    this->dbMan = dbMan;
    this->size = size;
}

QString
SqlImageModel::getId(const QModelIndex& index) const
{
    // 0: id
    return record(index.row()).value(0).toString();
}

QString
SqlImageModel::getDir(const QModelIndex& index) const
{
    // 1: dir
    return record(index.row()).value(1).toString();
}

QString
SqlImageModel::getName(const QModelIndex& index) const
{
    // 2: name
    return record(index.row()).value(2).toString();
}

QString
SqlImageModel::getPath(const QModelIndex& index) const
{
    const QString dir = getDir(index);
    const QString name = getName(index);
    const QString path = dbMan->pathAppend(dir, name);
    return path;
}

void
SqlImageModel::imageRowDoubleClicked(const QModelIndex& index)
{
    QString path = getPath(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

QVariant
SqlImageModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DecorationRole && index.column() == 0) {
        QString path = getPath(index);
        if (!QFile::exists(path)) {
            dbMan->deleteImage(getId(index));
            return QVariant();
        }
        QPixmap icon;
        if (QPixmapCache::find(path, &icon)) {
            return icon;
        }
        icon.load(path);
        icon = icon.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation);
        QPixmapCache::insert(path, icon);
        return icon;
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return "";
            case 1:
                return getName(index);
        }
    }
    return QSqlQueryModel::data(index, role);
}

Qt::ItemFlags
SqlImageModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QSqlQueryModel::flags(index);
    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }
    return defaultFlags;
}

QMimeData*
SqlImageModel::mimeData(const QModelIndexList& indexes) const
{
    if (indexes.isEmpty()) {
        return Q_NULLPTR;
    }
    QMimeData* data = new QMimeData();
    QList<QUrl> files;
    foreach (const QModelIndex& index, indexes) {
        if (index.column() != 0) {
            continue;
        }
        QString path = getPath(index);
        files += QUrl::fromLocalFile(path);
    }
    data->setUrls(files);
    return data;
}

bool
SqlImageModel::dropMimeData(const QMimeData* data,
                            Qt::DropAction action,
                            int row,
                            int column,
                            const QModelIndex& parent)
{
    // These variables aren't used
    (void)action;
    (void)row;
    (void)column;
    const QString imageId = getId(parent);
    const QString mimeType = "application/x-qabstractitemmodeldatalist";
    const QString tagsData = data->data(mimeType);
    const QStringList tagIds = tagsData.split(';');
    dbMan->mapTagsToImage(tagIds, imageId);
    return true;
}
