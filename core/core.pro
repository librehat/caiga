include(../common.pri)

QT       += widgets sql

SOURCES  += image.cpp\
            project.cpp \
            workerthread.cpp \
    workhistequalise.cpp \
    workspace.cpp \
    workmedianblur.cpp \
    workaptbilateralfilter.cpp \
    workbinaryzation.cpp \
    workcanny.cpp \
    workcontours.cpp

HEADERS  += image.h \
            project.h \
            core_lib.h \
            datastructure.h \
            workerthread.h \
            workspace.h \
    workbase.h \
    workhistequalise.h \
    workmedianblur.h \
    workaptbilateralfilter.h \
    workbinaryzation.h \
    workcanny.h \
    workcontours.h

DEFINES  += CORE
