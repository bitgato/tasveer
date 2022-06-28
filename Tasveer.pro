QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

win32:RC_ICONS = images/icon.ico
macx:ICON = images/icon.icns

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

unix: QMAKE_SUBSTITUTES += Tasveer.desktop.in
unix: desktop.path = $$PREFIX/share/applications/
unix: desktop.files = Tasveer.desktop
unix: icons.path = $$PREFIX/share/icons/hicolor/256x256/apps/
unix: icons.files = images/Tasveer.png
unix: INSTALLS += desktop icons

INCLUDEPATH += include/

MOC_DIR = ./build/moc
OBJECTS_DIR = ./build/obj
RCC_DIR = ./build/qrc
UI_DIR = ./build/uic

SOURCES += \
    src/databasemanager.cpp \
    src/imageloader.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/sqlimagemodel.cpp \
    src/sqltagmodel.cpp

HEADERS += \
    include/databasemanager.h \
    include/filetypes.h \
    include/imageloader.h \
    include/mainwindow.h \
    include/sqlimagemodel.h \
    include/sqltagmodel.h

FORMS += \
    ui/entertagsdialog.ui \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    tasveer.qrc

VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_BUILD = 0

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

DESCRIPTION = \\\"\"A simple image browsing and tagging application\"\\\"
LICENSE = \\\"GPLv3\\\"
LICENSE_URL = \\\"https://raw.githubusercontent.com/bitgato/tasveer/main/LICENSE\\\"
BUG_REPORT_URL = \\\"https://github.com/bitgato/tasveer/issues\\\"

DEFINES += \
    "VERSION_MAJOR=$$VERSION_MAJOR" \
    "VERSION_MINOR=$$VERSION_MINOR" \
    "VERSION_BUILD=$$VERSION_BUILD" \
    "DESCRIPTION=$$DESCRIPTION" \
    "LICENSE=$$LICENSE" \
    "LICENSE_URL=$$LICENSE_URL" \
    "BUG_REPORT_URL=$$BUG_REPORT_URL"
