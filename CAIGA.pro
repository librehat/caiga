#-------------------------------------------------
#
#                  Project CAIGA
#
#-------------------------------------------------

QT       += core gui widgets multimedia multimediawidgets

TARGET   = CAIGA
TEMPLATE = app


SOURCES  += main.cpp\
            mainwindow.cpp \
            cameradialog.cpp \
            image.cpp \
            project.cpp \
            optionsdialog.cpp \
            qimageviewer.cpp

HEADERS  += mainwindow.h \
            cameradialog.h \
            caigaglobal.h \
            image.h \
            project.h \
            optionsdialog.h \
            qimageviewer.h

FORMS    += mainwindow.ui \
            cameradialog.ui \
            optionsdialog.ui

RESOURCES+= caiga.qrc

windows: {
RESOURCES+= icons.qrc
RC_ICONS = caiga.ico

#Change lines below according to your setup environment
INCLUDEPATH += C:/openCV/build/include
LIBS += -LC:/openCV/build/x86/mingw/lib \
        -lopencv_core248 \
        -lopencv_imgproc248 \
        -lopencv_highgui248 \
        -lopencv_ml248 \
        -lopencv_video248 \
        -lopencv_features2d248 \
        -lopencv_calib3d248 \
        -lopencv_objdetect248 \
        -lopencv_contrib248 \
        -lopencv_legacy248 \
        -lopencv_flann248
} else:unix: {
CONFIG    += link_pkgconfig
PKGCONFIG += opencv
}
