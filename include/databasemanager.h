#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QProgressDialog>
#include <QSqlQuery>

class DatabaseManager
{
  private:
    QString CONNECTION;
    QString dbPath;
    QString dbName;
    void createTables();
    void escape(QString& text);

  public:
    DatabaseManager();
    QString pathAppend(const QString& p1, const QString& p2);
    QSqlQuery getAllDirs();
    QSqlQuery getAllImages();
    QSqlQuery getAllTags();
    QSqlQuery getTagsByImage(const QString& id);
    QSqlQuery getImagesByDir(const QString& dirPath);
    QSqlQuery filterImages(const QStringList& tagIds,
                           const QString& method,
                           QString& dir,
                           QString& name);
    QSqlQuery filterTags(const QString& method, QString& text);
    bool addDirectory(QProgressDialog& progress, const QString& dirPath);
    void addImage(const QString& path);
    void addTags(const QStringList& tags);
    void removeTags(const QString& imageId, const QStringList& tags);
    void deleteImage(const QString& imageId);
    void mapTagsToImage(const QStringList& tagIds, const QString& imageId);
};

#endif // DATABASEMANAGER_H
