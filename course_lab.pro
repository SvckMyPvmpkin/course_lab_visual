#-------------------------------------------------
#
# Project created by QtCreator 2025-11-05T10:48:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = course_lab
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Use modern C++
CONFIG += c++17

# Sources
SOURCES += \
        main.cpp \
        mainwindow.cpp \
        book.cpp \
        bookcollection.cpp \
        bookdialog.cpp \
        librarymanager.cpp \
        sortstrategy.cpp

# Headers (needed for moc on Q_OBJECT classes)
HEADERS += \
        mainwindow.h \
        book.h \
        bookcollection.h \
        bookdialog.h \
        librarymanager.h \
        sortstrategy.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
