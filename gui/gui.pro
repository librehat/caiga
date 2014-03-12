include(../caiga.pri)

QT    += gui widgets multimedia multimediawidgets sql

FORMS += cameradialog.ui \
         mainwindow.ui \
         optionsdialog.ui
         
SOURCES += cameradialog.cpp \
           mainwindow.cpp \
           optionsdialog.cpp \
           qimageviewer.cpp
           
HEADERS += cameradialog.h \
           mainwindow.h \
           optionsdialog.h \
           qimageviewer.h
