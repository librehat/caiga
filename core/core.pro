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
            workwatershed.cpp \
            analyser.cpp \
            workgradient.cpp \
            macro.cpp \
            ccspace.cpp \
    workscharr.cpp \
    workautowatershed.cpp

HEADERS  += image.h \
            core_lib.h \
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
            workwatershed.h \
            analyser.h \
            workgradient.h \
            macro.h \
            ccspace.h \
    workscharr.h \
    workautowatershed.h

DEFINES  += CORE
