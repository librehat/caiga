include(3rd_lib.pri)
include(../caiga.pri)

QT       += core gui widgets multimedia multimediawidgets sql

TEMPLATE = app

TARGET   = $$APP_TARGET

SOURCES  += main.cpp\
            mainwindow.cpp \
            cameradialog.cpp \
            image.cpp \
            project.cpp \
            optionsdialog.cpp \
            qimageviewer.cpp

HEADERS  += mainwindow.h \
            cameradialog.h \
            caigaglobal.h \
            image.h \
            project.h \
            optionsdialog.h \
            qimageviewer.h \
            abstractanalyser.h

FORMS    += mainwindow.ui \
            cameradialog.ui \
            optionsdialog.ui

RESOURCES+= ../icons/caiga.qrc

windows: {
    RESOURCES   += ../icons/icons.qrc
    RC_FILE      = winicon.rc
    OTHER_FILES += winicon.rc
}