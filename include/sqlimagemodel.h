#ifndef SQLIMAGEMODEL_H
#define SQLIMAGEMODEL_H

#include "databasemanager.h"

#include <QMap>
#include <QMimeData>
#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QSqlQueryModel>

class SqlImageModel : public QSqlQueryModel
{
    Q_OBJECT

  private:
    QSize size;
    QPixmap loadingIcon;
    QPixmap deletedIcon;
    DatabaseManager* dbMan;
    mutable QMap<QString, bool> loading;
    QString getDir(const QModelIndex& index) const;
    QString getPath(const QModelIndex& index) const;

  public:
    SqlImageModel(DatabaseManager* dbMan,
                  const QSize& size,
                  QObject* parent = nullptr);
    QString getId(const QModelIndex& index) const;
    QString getName(const QModelIndex& index) const;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;
    bool dropMimeData(const QMimeData* data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex& parent);
  public slots:
    void imageRowDoubleClicked(const QModelIndex& index);
    void cacheImage(const QString& path,
                    const QModelIndex& index,
                    const QImage& image);
};

#endif // SQLIMAGEMODEL_H
