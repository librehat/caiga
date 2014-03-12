#-------------------------------------------------
#
#                  Project CAIGA
#
#-------------------------------------------------
include(common.pri)

#Check Qt
!minQtVersion(5, 0, 0) {
    message("Cannot build CAIGA with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.0.0.")
}

TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS   = core \
            gui
