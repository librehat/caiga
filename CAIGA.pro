#-------------------------------------------------
#
#                  Project CAIGA
#
#-------------------------------------------------
include(common.pri)

!isEqual(QT_MAJOR_VERSION, 5) {
    message("Cannot build CAIGA with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.3.0.")
}
!greaterThan(QT_MINOR_VERSION, 2) {
    message("Cannot build CAIGA with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.3.0.")
}

TRANSLATIONS  = translations/caiga_zh_CN.ts

TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS   = core \
            gui
