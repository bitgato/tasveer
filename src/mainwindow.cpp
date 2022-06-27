#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QComboBox>
#include <QEvent>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPixmapCache>
#include <QProgressDialog>
#include <QShortcut>
#include <QSize>
#include <QSizePolicy>
#include <QTableView>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tagSearchBox->installEventFilter(this);
    ui->imageSearchBox->installEventFilter(this);
    QObject::connect(ui->methodBox,
                     &QComboBox::currentTextChanged,
                     this,
                     &MainWindow::filterTags);
    QObject::connect(ui->dirSelectBox,
                     &QComboBox::currentTextChanged,
                     this,
                     &MainWindow::filterImages);
    QObject::connect(ui->filterButton,
                     &QPushButton::released,
                     this,
                     &MainWindow::filterImages);
    QObject::connect(ui->actionAddDirectory,
                     &QAction::triggered,
                     this,
                     &MainWindow::showAddDirDialog);
    QObject::connect(ui->actionAddImage,
                     &QAction::triggered,
                     this,
                     &MainWindow::showAddImageDialog);
    QObject::connect(ui->actionAddTags,
                     &QAction::triggered,
                     this,
                     &MainWindow::showAddTagsDialog);
    QObject::connect(
      ui->actionQuit, &QAction::triggered, this, &QApplication::quit);
    QObject::connect(
      ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    QPixmapCache::setCacheLimit(102400);

    ui->splitter = new QSplitter(this);
    ui->splitter->setGeometry(50, 50, 0, 0);
    ui->splitter->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    ui->leftFrame->layout()->setContentsMargins(0, 0, 0, 0);
    ui->rightFrame->layout()->setContentsMargins(0, 0, 0, 0);
    ui->searchFrame->layout()->setContentsMargins(0, 0, 0, 0);
    ui->filterFrame->layout()->setContentsMargins(0, 0, 0, 0);

    QSize size = this->size();
    int newHeight = size.height() / 5;
    size.setHeight(newHeight);

    // Images
    imageModel = new SqlImageModel(&dbMan, size, this);
    QSqlQuery query = dbMan.getAllImages();
    imageModel->setQuery(query);
    while (imageModel->canFetchMore()) {
        imageModel->fetchMore();
    }
    imageModel->setHeaderData(
      0, Qt::Horizontal, tr("Preview"), Qt::DisplayRole);
    imageModel->setHeaderData(
      1, Qt::Horizontal, tr("Filename"), Qt::DisplayRole);
    ui->images->verticalHeader()->hide();
    ui->images->verticalHeader()->setDefaultSectionSize(newHeight);
    ui->images->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->images->setDragEnabled(true);
    ui->images->setAcceptDrops(true);
    ui->images->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->images,
                     &QTableView::doubleClicked,
                     imageModel,
                     &SqlImageModel::imageRowDoubleClicked);
    QObject::connect(ui->images,
                     &QTableView::customContextMenuRequested,
                     this,
                     &MainWindow::showImageContextMenu);
    ui->images->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->images->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->images->setModel(imageModel);
    ui->images->hideColumn(2);
    query.clear();

    // Tags
    tagModel = new SqlTagModel(this);
    query = dbMan.getAllTags();
    tagModel->setQuery(query);
    while (tagModel->canFetchMore()) {
        tagModel->fetchMore();
    }
    ui->tagsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tagsList->setDragEnabled(true);
    ui->tagsList->setModel(tagModel);
    query.clear();

    // Directories
    query = dbMan.getAllDirs();
    while (query.next()) {
        ui->dirSelectBox->addItem(query.value(0).toString());
    }
    query.finish();

    // Shortcuts
    auto tagBoxFocusShortcut =
      new QShortcut(QKeySequence(tr("Ctrl+S", "Focus|Tag box")), this);
    QObject::connect(tagBoxFocusShortcut,
                     &QShortcut::activated,
                     this,
                     &MainWindow::setTagBoxFocus);

    auto imageBoxFocusShortcut =
      new QShortcut(QKeySequence(tr("Ctrl+Z", "Focus|Image box")), this);
    QObject::connect(imageBoxFocusShortcut,
                     &QShortcut::activated,
                     this,
                     &MainWindow::setImageBoxFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::addTags()
{
    dbMan.addTags(tagsChanged.keys());
    filterTags(ui->methodBox->currentText());
}

void
MainWindow::removeTags()
{
    dbMan.removeTags(tagRemoveImageId, tagsChanged.keys());
    filterTags(ui->methodBox->currentText());
}

void
MainWindow::removeNewTag()
{
    QPushButton* btn = static_cast<QPushButton*>(sender());
    enterTagsUi.enteredTagsList->removeWidget(btn);
    tagsChanged.remove(btn->text());
    delete btn;
}

void
MainWindow::removeExistingTag()
{
    QPushButton* btn = static_cast<QPushButton*>(sender());
    showTagsUi.enteredTagsList->removeWidget(btn);
    tagsChanged[btn->text()] = true;
    delete btn;
}

void
MainWindow::filterImages()
{
    QString dirFilter = ui->dirSelectBox->currentText();
    if (dirFilter == "All") {
        dirFilter = "";
    }
    QString nameFilter = ui->imageSearchBox->text().trimmed();
    QString method = ui->filterMethodBox->currentText();
    QStringList tagIds;
    foreach (const QModelIndex& index,
             ui->tagsList->selectionModel()->selectedIndexes()) {
        QString tagId = tagModel->getId(index);
        tagIds += tagId;
    }
    QSqlQuery query = dbMan.filterImages(tagIds, method, dirFilter, nameFilter);
    imageModel->setQuery(query);
}

void
MainWindow::filterTags(const QString& method)
{
    QString text = ui->tagSearchBox->text();
    QSqlQuery query = dbMan.filterTags(method, text);
    tagModel->setQuery(query);
}

void
MainWindow::showAddDirDialog()
{
    auto flags = QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;
    QString dir =
      QFileDialog::getExistingDirectory(this, "Open Directory", "/home", flags);
    QProgressDialog progress("Adding directory", "Cancel", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    if (dbMan.addDirectory(progress, dir)) {
        ui->dirSelectBox->addItem(dir);
    }
    filterImages();
}

void
MainWindow::showAddImageDialog()
{
    QString fileTypes = "Image files (*.jpg *.jpeg *.png *.gif *.webp)";
    QString file =
      QFileDialog::getOpenFileName(this, "Open image", "/home", fileTypes);
    dbMan.addImage(file);
    filterImages();
}

void
MainWindow::showAddTagsDialog()
{
    tagsChangedCount = 0;
    tagsChanged.clear();
    enterTagsDialog = new QDialog(this);
    enterTagsDialog->deleteLater();
    enterTagsUi.setupUi(enterTagsDialog);
    enterTagsUi.enterTagsBox->installEventFilter(this);
    QObject::connect(
      enterTagsDialog, &QDialog::accepted, this, &MainWindow::addTags);
    enterTagsDialog->exec();
}

void
MainWindow::showShowTagsDialog()
{
    tagsChangedCount = 0;
    tagsChanged.clear();
    QModelIndexList selected = ui->images->selectionModel()->selectedRows();
    showTagsDialog = new QDialog(this);
    showTagsDialog->deleteLater();
    showTagsUi.setupUi(showTagsDialog);
    if (selected.empty()) {
        return;
    }
    tagRemoveImageId = imageModel->getId(selected[0]);
    QSqlQuery tags = dbMan.getTagsByImage(tagRemoveImageId);
    while (tags.next()) {
        QString tagText = tags.value(1).toString();
        QPushButton* btn = new QPushButton(tagText, showTagsUi.scrollArea);
        QObject::connect(
          btn, &QPushButton::clicked, this, &MainWindow::removeExistingTag);
        showTagsUi.enteredTagsList->addWidget(
          btn, tagsChangedCount / 4, tagsChangedCount % 4);
        ++tagsChangedCount;
    }
    showTagsUi.enterTagsBox->installEventFilter(this);
    showTagsUi.enterTagsBox->hide();
    showTagsUi.label->setText("Click on tags and confirm to remove them");
    QObject::connect(
      showTagsDialog, &QDialog::accepted, this, &MainWindow::removeTags);
    showTagsDialog->exec();
}

void
MainWindow::showImageContextMenu(const QPoint& pos)
{
    QMenu contextMenu(tr("Context Menu"), ui->images);
    QAction showTags("Show Tags", ui->images);
    QObject::connect(
      &showTags, &QAction::triggered, this, &MainWindow::showShowTagsDialog);
    contextMenu.addAction(&showTags);
    contextMenu.exec(ui->images->viewport()->mapToGlobal(pos));
}

void
MainWindow::showAboutDialog()
{
    QMessageBox::about(this, "About Tasveer", ABOUT);
}

void
MainWindow::setTagBoxFocus()
{
    ui->tagSearchBox->setFocus();
}

void
MainWindow::setImageBoxFocus()
{
    ui->imageSearchBox->setFocus();
}

bool
MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    bool result = QObject::eventFilter(watched, event);
    if (watched == ui->tagSearchBox && event->type() == QEvent::KeyRelease) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            ui->tagSearchBox->clearFocus();
            return result;
        }
        filterTags(ui->methodBox->currentText());
    }
    if (watched == ui->imageSearchBox && event->type() == QEvent::KeyRelease) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            ui->imageSearchBox->clearFocus();
            return result;
        }
        filterImages();
    }
    if (enterTagsDialog && watched == enterTagsUi.enterTagsBox &&
        event->type() == QEvent::KeyRelease) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() != Qt::Key_Semicolon) {
            return result;
        }
        QString tagText = enterTagsUi.enterTagsBox->text();
        tagText = tagText.replace(";", "").simplified();
        // Skip the tag if it has been added as a button already
        if (tagsChanged[tagText]) {
            enterTagsUi.enterTagsBox->setText("");
            return result;
        }
        if (tagText != "") {
            QPushButton* btn = new QPushButton(tagText, enterTagsUi.scrollArea);
            QObject::connect(
              btn, &QPushButton::clicked, this, &MainWindow::removeNewTag);
            enterTagsUi.enteredTagsList->addWidget(
              btn, tagsChangedCount / 4, tagsChangedCount % 4);
            ++tagsChangedCount;
            tagsChanged[tagText] = true;
        }
        enterTagsUi.enterTagsBox->setText("");
    }
    return result;
}
