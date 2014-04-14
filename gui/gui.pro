include(../common.pri)

QT       += gui widgets multimedia multimediawidgets sql

TEMPLATE  = app

TARGET    = caiga

FORMS    += cameradialog.ui \
            mainwindow.ui \
            optionsdialog.ui
         
SOURCES  += main.cpp \
            cameradialog.cpp \
            mainwindow.cpp \
            optionsdialog.cpp \
            qimageviewer.cpp \
    qimagedrawer.cpp
           
HEADERS  += cameradialog.h \
            mainwindow.h \
            optionsdialog.h \
            qimageviewer.h \
    qimagedrawer.h

RESOURCES+= caiga.qrc
windows: {
    RESOURCES   += icons.qrc
    RC_FILE      = winicon.rc
    OTHER_FILES += winicon.rc
}

windows:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lcore
else:windows:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lcore
else:unix: LIBS += -L$$OUT_PWD/../core/ -lcore

INCLUDEPATH += $$PWD/../core
DEPENDPATH  += $$PWD/../core
