#ifndef SQLTAGMODEL_H
#define SQLTAGMODEL_H

#include <QMimeData>
#include <QSqlQueryModel>

class SqlTagModel : public QSqlQueryModel
{
  private:
    QString getTag(const QModelIndex& index) const;

  public:
    SqlTagModel(QObject* parent = nullptr);
    QString getId(const QModelIndex& index) const;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;
};

#endif // SQLTAGMODEL_H
