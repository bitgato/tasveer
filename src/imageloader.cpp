#include "imageloader.h"

ImageLoader::ImageLoader(QObject* parent)
  : QObject(parent)
{}

void
ImageLoader::setSize(const QSize& size)
{
    this->size = size;
}

void
ImageLoader::setPath(const QString& path)
{
    this->path = path;
}

void
ImageLoader::setIndex(const QModelIndex& index)
{
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
