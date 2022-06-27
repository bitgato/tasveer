#include "databasemanager.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSqlError>
#include <QStandardPaths>
#include <QStringList>

DatabaseManager::DatabaseManager()
{
    CONNECTION = "SQLITE";
    dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dbDir(dbPath);
    if (!dbDir.exists()) {
        dbDir.mkdir(dbPath);
    }
    dbPath = pathAppend(dbPath, "tasveer.db");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", CONNECTION);
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qDebug() << "connectError: " << db.lastError().text();
        return;
    }
    createTables();
}

QString
DatabaseManager::pathAppend(const QString& p1, const QString& p2)
{
    return QDir::cleanPath(p1 + QDir::separator() + p2);
}

void
DatabaseManager::escape(QString& text)
{
    text.replace("@", "@@");
    text.replace("_", "@_");
    text.replace("%", "@%");
}

void
DatabaseManager::createTables()
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    query.prepare("PRAGMA foreign_keys = ON");
    if (!query.exec()) {
        qDebug() << "fkPragmaONError: " << query.lastError().text();
        return;
    }
    query.prepare("CREATE TABLE "
                  "IF NOT EXISTS "
                  "images "
                  "(image_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "dir VARCHAR(10) NOT NULL,"
                  "name VARCHAR(10) NOT NULL,"
                  "UNIQUE (dir, name) ON CONFLICT IGNORE)");
    if (!query.exec()) {
        qDebug() << "imagesTableCreateError: " << query.lastError().text();
        return;
    }
    query.prepare("CREATE TABLE "
                  "IF NOT EXISTS "
                  "tags "
                  "(tag_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "tag VARCHAR(10) NOT NULL UNIQUE ON CONFLICT IGNORE)");
    if (!query.exec()) {
        qDebug() << "tagsTableCreateError: " << query.lastError().text();
        return;
    }
    query.prepare("CREATE TABLE "
                  "IF NOT EXISTS "
                  "tag_image_map "
                  "(image_id INTEGER REFERENCES images(image_id) "
                  "ON DELETE CASCADE,"
                  "tag_id INTEGER REFERENCES tags(tag_id) "
                  "ON DELETE CASCADE,"
                  "UNIQUE(image_id, tag_id) "
                  "ON CONFLICT IGNORE)");
    if (!query.exec()) {
        qDebug() << "mapTableCreateError: " << query.lastError().text();
    }
}

QSqlQuery
DatabaseManager::getAllDirs()
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    query.prepare("SELECT DISTINCT dir FROM images");
    if (!query.exec()) {
        qDebug() << "selectAllDirsError: " << query.lastError().text();
    }
    return query;
}

QSqlQuery
DatabaseManager::getAllImages()
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM images "
                  "ORDER BY name");
    if (!query.exec()) {
        qDebug() << "selectAllImagesError: " << query.lastError().text();
    }
    return query;
}

QSqlQuery
DatabaseManager::getAllTags()
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM tags ORDER BY tag");
    if (!query.exec()) {
        qDebug() << "selectAllTagsError: " << query.lastError().text();
    }
    return query;
}

QSqlQuery
DatabaseManager::getTagsByImage(const QString& imageId)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    query.prepare("SELECT t.tag_id, t.tag "
                  "FROM tag_image_map tim "
                  "JOIN tags t ON tim.tag_id = t.tag_id "
                  "WHERE tim.image_id = (:imageId)");
    query.bindValue(":imageId", imageId);
    if (!query.exec()) {
        qDebug() << "selectTagsByImageError: " << query.lastError().text();
    }
    return query;
}

QSqlQuery
DatabaseManager::getImagesByDir(const QString& dirPath)
{
    QDir dir(dirPath);
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    if (!dir.exists() || dir.isEmpty()) {
        return query;
    }
    QString absDirPath = dir.absolutePath();
    query.prepare("SELECT * FROM images "
                  "WHERE dir = (:dirPath) "
                  "ORDER BY NAME");
    query.bindValue(":dirPath", absDirPath);
    if (!query.exec()) {
        qDebug() << "queryDirError: " << query.lastError().text();
    }
    return query;
}

QSqlQuery
DatabaseManager::filterImages(const QStringList& tagIds,
                              const QString& method,
                              QString& dir,
                              QString& name)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    QString queryString;

    bool dirGiven = dir != "";
    bool nameGiven = name != "";
    if (dirGiven) {
        escape(dir);
    }
    if (nameGiven) {
        escape(name);
    }

    int count = tagIds.size();
    if (count == 0) {
        queryString = "SELECT * FROM images";
        if (nameGiven || dirGiven) {
            queryString += " WHERE ";
        }
        if (dirGiven) {
            queryString += "dir LIKE '%' || (?) || '%'";
        }
        if (nameGiven) {
            if (dirGiven)
                queryString += " AND ";
            queryString += "name LIKE '%' || (?) || '%'";
        }
        if (nameGiven || dirGiven) {
            queryString += " ESCAPE '@'";
        }
        queryString += " ORDER BY name";
        query.prepare(queryString);
        if (dirGiven) {
            query.bindValue(0, dir);
        }
        if (nameGiven) {
            query.bindValue(dirGiven ? 1 : 0, name);
        }
    }
    // If there is only 1 tag to filter, the method doesn't matter
    else if (method == "OR" || count == 1) {
        queryString = "SELECT DISTINCT i.image_id, i.dir, i.name "
                      "FROM tag_image_map tim "
                      "JOIN images i "
                      "ON tim.image_id = i.image_id "
                      "WHERE (tim.tag_id = (?) ";
        for (int i = 1; i < count; i++) {
            queryString += "OR tim.tag_id = (?) ";
        }
        queryString += ") ";
        if (dirGiven) {
            queryString += "AND i.dir LIKE '%' || (?) || '%' ";
        }
        if (nameGiven) {
            queryString += "AND i.name LIKE '%' || (?) || '%' ";
        }
        if (nameGiven || dirGiven) {
            queryString += "ESCAPE '@' ";
        }
        queryString += "ORDER BY i.name";
        query.prepare(queryString);
        for (int i = 0; i < count; i++) {
            query.bindValue(i, tagIds[i]);
        }
        if (dirGiven) {
            query.bindValue(count++, dir);
        }
        if (nameGiven) {
            query.bindValue(count, name);
        }
    } else {
        queryString = "WITH cte(tim_ii, i_ii, i_d, i_n) AS "
                      "("
                      "SELECT tim.image_id, i.image_id, i.dir, i.name "
                      "FROM tag_image_map tim "
                      "JOIN images i ON tim.image_id = i.image_id "
                      "WHERE tim.tag_id = (?)";
        for (int i = 1; i < count; i++) {
            queryString += " OR tim.tag_id = (?)";
        }
        queryString += ")"
                       "SELECT i_ii, i_d, i_n "
                       "FROM cte GROUP BY tim_ii "
                       "HAVING count() = (?) ";
        if (dirGiven) {
            queryString += "AND i_d LIKE '%' || (?) || '%' ";
        }
        if (nameGiven) {
            queryString += "AND i_n LIKE '%' || (?) || '%' ";
        }
        if (nameGiven || dirGiven) {
            queryString += "ESCAPE '@' ";
        }
        queryString += "ORDER BY i_n";
        query.prepare(queryString);
        for (int i = 0; i < count; i++) {
            query.bindValue(i, tagIds[i]);
        }
        query.bindValue(count, count);
        ++count;
        if (dirGiven) {
            query.bindValue(count++, dir);
        }
        if (nameGiven) {
            query.bindValue(count, name);
        }
    }
    if (!query.exec()) {
        qDebug() << "filterImagesError: " << query.lastError().text();
    }
    return query;
}

QSqlQuery
DatabaseManager::filterTags(const QString& method, QString& text)
{
    QStringList tags = text.split(";");
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    QString queryString = "SELECT * FROM tags";

    int index = 0, length = tags.length();
    for (int i = 0; i < length; i++) {
        tags[i] = tags[i].trimmed();
        if (tags[i] == "") {
            continue;
        }

        escape(tags[i]);

        if (index == 0) {
            queryString += " WHERE ";
        } else if (method == "AND") {
            queryString += " AND ";
        } else {
            queryString += " OR ";
        }

        queryString += "tag LIKE '%' || (?) || '%'";
        index++;
    }
    // If any placeholders were set (LIKE statement was added)
    if (index > 0) {
        queryString += " ESCAPE '@'";
    }
    queryString += " ORDER BY tag";
    query.prepare(queryString);
    // If there are placeholders, bind values. Else the query
    // is still 'SELECT tag FROM tags' which selects all tags
    if (index > 0) {
        foreach (auto& tag, tags) {
            if (tag == "")
                continue;
            // Decrease the index first because it's initially
            // the 1-indexed number of parameters in the query
            // but we want 0-indexed for bindValue
            query.bindValue(--index, tag);
        }
    }
    if (!query.exec()) {
        qDebug() << "filterTagsError: " << query.lastError().text();
    }
    return query;
}

bool
DatabaseManager::addDirectory(QProgressDialog& progress, const QString& dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists() || dir.isEmpty()) {
        return false;
    }
    QStringList fileTypes;
    fileTypes << "*.jpg"
              << "*.jpeg"
              << "*.png"
              << "*.gif"
              << "*.webp";
    dir.setNameFilters(fileTypes);
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    if (!db.transaction()) {
        qDebug() << "dbDirTransactionError: " << db.lastError().text();
        return false;
    }
    QSqlQuery query(db);
    query.prepare("INSERT INTO "
                  "images "
                  "(dir, name) "
                  "VALUES (:dir, :name)");
    QDirIterator it(dirPath, fileTypes, QDir::Files);
    int done = 0, total = dir.count();
    while (it.hasNext()) {
        if (progress.wasCanceled()) {
            db.rollback();
            return false;
        }
        QFileInfo fi(it.next());
        QString absDirPath = fi.absolutePath();
        QString fileName = fi.fileName();
        query.bindValue(":dir", absDirPath);
        query.bindValue(":name", fileName);
        if (!query.exec()) {
            qDebug() << "insertDirError: " << query.lastError().text();
        }
        progress.setValue(done / total);
        ++done;
    }
    if (!db.commit()) {
        qDebug() << "dbDirCommitError: " << query.lastError().text();
        return false;
    }
    return true;
}

void
DatabaseManager::addImage(const QString& path)
{
    if (path.isEmpty()) {
        return;
    }
    QFileInfo fi(path);
    QString dir = fi.absolutePath();
    QString name = fi.fileName();
    if (name.isEmpty()) {
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    query.prepare("INSERT INTO images (dir, name) VALUES (:dir, :name)");
    query.bindValue(":dir", dir);
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << "insertError: " << query.lastError().text();
    }
}

void
DatabaseManager::addTags(const QStringList& tags)
{
    if (tags.empty()) {
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    if (!db.transaction()) {
        qDebug() << "addTagsTransactionError: " << db.lastError().text();
        return;
    }
    QSqlQuery query(db);
    query.prepare("INSERT INTO tags(tag) VALUES (:tag)");
    int size = tags.size();
    for (int i = 0; i < size; i++) {
        query.bindValue(":tag", tags[i]);
        if (!query.exec()) {
            qDebug() << "addTagsInsertError: " << query.lastError().text();
        }
    }
    if (!db.commit()) {
        qDebug() << "addTagsCommitError: " << query.lastError().text();
        return;
    }
}

void
DatabaseManager::removeTags(const QString& imageId, const QStringList& tags)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    if (!db.transaction()) {
        qDebug() << "removeTagsTransactionError: " << db.lastError().text();
        return;
    }
    QSqlQuery query(db);
    query.prepare("DELETE FROM tag_image_map "
                  "WHERE ROWID IN "
                  "("
                  "SELECT tim.ROWID FROM tag_image_map tim "
                  "JOIN tags t "
                  "ON (t.tag = (:tag) AND tim.image_id = (:imageId) "
                  "AND tim.tag_id = t.tag_id)"
                  ")");
    foreach (auto& tag, tags) {
        query.bindValue(":tag", tag);
        query.bindValue(":imageId", imageId);
        if (!query.exec()) {
            qDebug() << query.executedQuery();
            qDebug() << "removeTagsExecError: " << query.lastError().text();
        }
    }
    if (!db.commit()) {
        qDebug() << "removeTagsCommitError: " << db.lastError().text();
        return;
    }
}

void
DatabaseManager::deleteImage(const QString& imageId)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    QSqlQuery query(db);
    query.prepare("DELETE FROM images WHERE image_id = (:imageId)");
    query.bindValue(":imageId", imageId);
    if (!query.exec()) {
        qDebug() << "deleteImageError: " << query.lastError().text();
    }
}

void
DatabaseManager::mapTagsToImage(const QStringList& tagIds,
                                const QString& imageId)
{
    QSqlDatabase db = QSqlDatabase::database(CONNECTION);
    if (!db.transaction()) {
        qDebug() << "mapInsertTransactionError: " << db.lastError().text();
        return;
    }
    QSqlQuery query = QSqlQuery(db);
    query.prepare("INSERT INTO tag_image_map(image_id, tag_id) "
                  "VALUES(:imageId, :tagId)");
    for (auto tagId : tagIds) {
        query.bindValue(":imageId", imageId);
        query.bindValue(":tagId", tagId);
        if (!query.exec()) {
            qDebug() << "mapInsertBindError: " << db.lastError().text();
            return;
        }
    }
    if (!db.commit()) {
        qDebug() << "mapInsertCommitError: " << db.lastError().text();
    }
}
