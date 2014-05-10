include(../common.pri)

SOURCES  += image.cpp\
            workhistequalise.cpp \
            workspace.cpp \
            workmedianblur.cpp \
            workaptbilateralfilter.cpp \
            workbinaryzation.cpp \
            workcanny.cpp \
            workcontours.cpp \
            workfloodfill.cpp \
            workboxfilter.cpp \
            workpencil.cpp \
            workeraser.cpp \
            workinvertgrayscale.cpp \
            workwatershed.cpp

HEADERS  += image.h \
            core_lib.h \
            datastructure.h \
            workspace.h \
            workbase.h \
            workhistequalise.h \
            workmedianblur.h \
            workaptbilateralfilter.h \
            workbinaryzation.h \
            workcanny.h \
            workcontours.h \
            workfloodfill.h \
            workboxfilter.h \
            workpencil.h \
            workeraser.h \
            workinvertgrayscale.h \
            workwatershed.h

DEFINES  += CORE
