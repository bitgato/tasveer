#ifndef SQLIMAGEMODEL_H
#define SQLIMAGEMODEL_H

#include "databasemanager.h"

#include <QMimeData>
#include <QObject>
#include <QSize>
#include <QSqlQueryModel>

class SqlImageModel : public QSqlQueryModel
{
  private:
    QSize size;
    DatabaseManager* dbMan;
    QString getDir(const QModelIndex& index) const;
    QString getName(const QModelIndex& index) const;
    QString getPath(const QModelIndex& index) const;

  public:
    SqlImageModel(DatabaseManager* dbMan,
                  const QSize& size,
                  QObject* parent = nullptr);
    QString getId(const QModelIndex& index) const;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;
    bool dropMimeData(const QMimeData* data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex& parent);
    void imageRowDoubleClicked(const QModelIndex& index);
};

#endif // SQLIMAGEMODEL_H
