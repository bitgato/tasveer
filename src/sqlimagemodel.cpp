#include "imageloader.h"
#include "sqlimagemodel.h"

#include <QDesktopServices>
#include <QFile>
#include <QPixmap>
#include <QPixmapCache>
#include <QSqlRecord>
#include <QUrl>

SqlImageModel::SqlImageModel(DatabaseManager* dbMan,
                             QSize& size,
                             QObject* parent)
  : QSqlQueryModel(parent)
{
    this->dbMan = dbMan;
    this->size = size;
    this->loadingIcon.load(":/icons/images/loading.png");
    this->deletedIcon.load(":/icons/images/deleted.png");
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
    if (!QFile::exists(path)) {
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

QVariant
SqlImageModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DecorationRole && index.column() == 0) {
        QString path = getPath(index);
        // If the image is already being loaded
        if (loading[path]) {
            return loadingIcon;
        }
        if (!QFile::exists(path)) {
            dbMan->removeImage(getId(index));
            return deletedIcon;
        }
        QPixmap icon;
        if (QPixmapCache::find(path, &icon)) {
            return icon;
        }
        QThread* thread = new QThread();
        ImageLoader* loader = new ImageLoader();
        loader->setPath(path);
        loader->setSize(size);
        loader->setIndex(index);
        loader->moveToThread(thread);
        // Start image loading on thread start
        QObject::connect(
          thread, &QThread::started, loader, &ImageLoader::loadImage);
        // Update UI on image load
        QObject::connect(
          loader, &ImageLoader::imageLoaded, this, &SqlImageModel::cacheImage);
        // Delete `loader` later
        QObject::connect(
          loader, &ImageLoader::finished, loader, &ImageLoader::deleteLater);
        // Quit `thread` on image load and `loader` destroyed
        QObject::connect(
          loader, &ImageLoader::destroyed, thread, &QThread::quit);
        // Delete `thread` on finish
        QObject::connect(
          thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
        loading[path] = true;
        return loadingIcon;
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QVariant();
            case 1:
                return getName(index);
        }
    }
    return QSqlQueryModel::data(index, role);
}

void
SqlImageModel::cacheImage(const QString& path,
                          const QModelIndex& index,
                          const QImage& image)
{
    QPixmapCache::insert(path, QPixmap::fromImage(image));
    loading.remove(path);
    emit dataChanged(index, index, { Qt::DecorationRole });
}

Qt::ItemFlags
SqlImageModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QSqlQueryModel::flags(index);
    if (!index.isValid()) {
        return defaultFlags;
    }
    return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
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
