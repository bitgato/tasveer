#include "imageloader.h"

ImageLoader::ImageLoader(const QString& path,
                         const QSize& size,
                         const QModelIndex& index,
                         QObject* parent)
  : QObject(parent)
{
    this->path = path;
    this->size = size;
    this->index = index;
}

void
ImageLoader::loadImage()
{
    QImageReader reader(path);
    QSize imgSize = reader.size();
    double imgHeight = static_cast<double>(imgSize.height());
    double imgWidth = static_cast<double>(imgSize.width());
    double ratio = imgWidth / imgHeight;
    imgSize.rwidth() = ratio * size.height();
    imgSize.rheight() = size.height();
    reader.setScaledSize(imgSize);
    reader.setAutoTransform(true);
    QImage image = reader.read();
    emit finished();
    emit imageLoaded(path, index, image);
}
