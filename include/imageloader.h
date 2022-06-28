#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QImage>
#include <QImageReader>
#include <QModelIndex>
#include <QObject>
#include <QSize>

class ImageLoader : public QObject
{
    Q_OBJECT
  private:
    QSize size;
    QString path;
    QModelIndex index;

  public:
    ImageLoader(QObject* parent = nullptr);
    void setSize(const QSize& size);
    void setPath(const QString& path);
    void setIndex(const QModelIndex& index);
  signals:
    void finished();
    void imageLoaded(const QString& path,
                     const QModelIndex& index,
                     const QImage& image);
  public slots:
    void loadImage();
};

#endif // IMAGELOADER_H
