include(../common.pri)

QT       += widgets sql

SOURCES  += image.cpp\
            project.cpp \
            workerthread.cpp \
    workhistequalise.cpp \
    workspace.cpp \
    workmedianblur.cpp \
    workaptbilateralfilter.cpp

HEADERS  += image.h \
            project.h \
            core_lib.h \
            datastructure.h \
            workerthread.h \
            workspace.h \
    workbase.h \
    workhistequalise.h \
    workmedianblur.h \
    workaptbilateralfilter.h

DEFINES  += CORE
