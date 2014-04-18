QT          += core concurrent

VERSION     = 0.1.0
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

    CONFIG(release, debug|release): {
    LIBS += -lopencv_core248 \
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
    } else: CONFIG(debug, release|debug): {
    LIBS += -lopencv_core248d \
            -lopencv_imgproc248d \
            -lopencv_highgui248d \
            -lopencv_ml248d \
            -lopencv_video248d \
            -lopencv_features2d248d \
            -lopencv_calib3d248d \
            -lopencv_objdetect248d \
            -lopencv_contrib248d \
            -lopencv_legacy248d \
            -lopencv_flann248d
    }
} else:unix: {
    CONFIG    += link_pkgconfig
    PKGCONFIG += opencv
}
