include(../common.pri)

QT       += gui multimedia multimediawidgets printsupport

windows: QT += winextras

TEMPLATE  = app

TARGET    = caiga

FORMS    += cameradialog.ui \
            mainwindow.ui \
            optionsdialog.ui \
            parametersdialog.ui \
            watershedmarkerdialog.ui
         
SOURCES  += main.cpp \
            cameradialog.cpp \
            mainwindow.cpp \
            optionsdialog.cpp \
            qimageviewer.cpp \
            qimagedrawer.cpp \
            qimageinteractivedrawer.cpp \
            parametersdialog.cpp \
            livepixelviewer.cpp \
            watershedmarkerdialog.cpp \
			3rdparty/qcustomplot.cpp
           
HEADERS  += cameradialog.h \
            mainwindow.h \
            optionsdialog.h \
            qimageviewer.h \
            qimagedrawer.h \
            qimageinteractivedrawer.h \
            parametersdialog.h \
            livepixelviewer.h \
            watershedmarkerdialog.h \
			3rdparty/qcustomplot.h

#static: {
#    QTPLUGIN += qico
#    DEFINES += STATIC
#    message("Static build.")
#}

RESOURCES+= caiga.qrc
windows: {
    RESOURCES   += icons.qrc
    RC_FILE      = winicon.rc
    OTHER_FILES += winicon.rc
}

windows:CONFIG(release, debug|release): LIBS += -L../core/release -lcore
else:windows:CONFIG(debug, debug|release): LIBS += -L../core/debug -lcore
else:unix: LIBS += -L../core -lcore

INCLUDEPATH += ../core
DEPENDPATH  += ../core
