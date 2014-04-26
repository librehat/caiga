include(../common.pri)

QT       += widgets sql

SOURCES  += image.cpp\
            project.cpp \
            workerthread.cpp

HEADERS  += image.h \
            project.h \
            core_lib.h \
            datastructure.h \
            workerthread.h

DEFINES  += CORE
