#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "databasemanager.h"
#include "sqlimagemodel.h"
#include "sqltagmodel.h"
#include "ui_entertagsdialog.h"

#include <QDialog>
#include <QMainWindow>
#include <QMap>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  protected:
    bool eventFilter(QObject* watched, QEvent* event);

  private:
    QString APP_NAME = QApplication::applicationName();
    // All the parameters are defined in the .pro file
    QString ABOUT = QString("<h2>%1 v%2.%3.%4</h2>"
                            "<p>%5</p>"
                            "<p>Released under <a href='%6'>%7</a></p>"
                            "<a href='%8'>Report a bug</a>")
                      .arg(APP_NAME,
                           QString::number(VERSION_MAJOR),
                           QString::number(VERSION_MINOR),
                           QString::number(VERSION_BUILD),
                           DESCRIPTION,
                           LICENSE_URL,
                           LICENSE,
                           BUG_REPORT_URL);
    int tagsChangedCount;
    QString tagRemoveImageId;
    QMap<QString, bool> tagsChanged;
    Ui::MainWindow* ui;
    Ui_Dialog enterTagsUi;
    Ui_Dialog showTagsUi;
    QDialog* enterTagsDialog = Q_NULLPTR;
    QDialog* showTagsDialog = Q_NULLPTR;
    SqlImageModel* imageModel;
    SqlTagModel* tagModel;
    DatabaseManager dbMan;
  private slots:
    void addTags();
    void removeTags();
    void removeNewTag();
    void removeExistingTag();
    void removeImage();
    void filterImages();
    void filterTags(const QString& method);
    void showAddDirDialog();
    void showAddImageDialog();
    void showAddTagsDialog();
    void showAboutDialog();
    void showImageContextMenu(const QPoint& pos);
    void showShowTagsDialog();
    void setTagBoxFocus();
    void setImageBoxFocus();
    void toggleUntaggedCheckbox();
    void disableUntaggedCheckbox();
};
#endif // MAINWINDOW_H
