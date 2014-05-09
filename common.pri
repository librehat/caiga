QT          += core concurrent widgets

unix: VERSION     = 0.1.0
TEMPLATE    = lib
UI_DIR      = uics
MOC_DIR     = mocs
OBJECTS_DIR = objs

windows: {
    #Change lines below according to your setup environment
    INCLUDEPATH += C:/openCV/build/include

    win32-msvc2012: {#assume building 64-bit version
        contains(QMAKE_HOST.arch, x86_64):{
        LIBS += -LC:/openCV/build/x64/vc11/lib
        }
        else: {
        LIBS += -LC:/openCV/build/x86/vc11/lib
        }
    } else:win32-g++ {
        LIBS += -LC:/openCV/build/x86/mingw/lib
    } else: {
        message("Unsupported Windows Compiler or Environment")
        error("Use MinGW G++ or Visual C++ Compiler")
    }
    LIBS += -lopencv_core247 \
            -lopencv_imgproc247 \
            -lopencv_highgui247 \
            -lopencv_ml247 \
            -lopencv_video247 \
            -lopencv_features2d247 \
            -lopencv_calib3d247 \
            -lopencv_objdetect247 \
            -lopencv_contrib247 \
            -lopencv_legacy247 \
            -lopencv_flann247
} else:unix: {
    CONFIG    += link_pkgconfig
    PKGCONFIG += opencv
}
