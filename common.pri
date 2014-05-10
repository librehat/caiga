QT          += core concurrent widgets

unix: VERSION     = 0.1.0
TEMPLATE    = lib
UI_DIR      = uics
MOC_DIR     = mocs
OBJECTS_DIR = objs

win32: {
    #Change lines below according to your setup environment
    INCLUDEPATH += C:/openCV/build/include

    static: {
        win32-msvc2012: {#assume building 64-bit version
            contains(QMAKE_HOST.arch, x86_64):{
                LIBS += -LC:/openCV/build/x64/vc11/staticlib
            }
            else: {
                LIBS += -LC:/openCV/build/x86/vc11/staticlib
            }
        } win32-msvc2013: {
            contains(QMAKE_HOST.arch, x86_64):{
                LIBS += -LC:/openCV/build/x64/vc12/staticlib
            }
            else: {
                LIBS += -LC:/openCV/build/x86/vc12/staticlib
            }
        } else:win32-g++: {
                LIBS += -LC:/openCV/build/x86/mingw/staticlib
        } else: {
            message("Unsupported Windows Compiler or Environment")
            error("Use MinGW G++ or Visual C++ Compiler")
        }
    } else: {
        win32-msvc2012: {#assume building 64-bit version
            contains(QMAKE_HOST.arch, x86_64): {
                LIBS += -LC:/openCV/build/x64/vc11/lib
            } else: {
                LIBS += -LC:/openCV/build/x86/vc11/lib
            }
        } win32-msvc2013: {
            contains(QMAKE_HOST.arch, x86_64):{
                LIBS += -LC:/openCV/build/x64/vc12/lib
            }
            else: {
                LIBS += -LC:/openCV/build/x86/vc12/lib
            }
        } else:win32-g++: {
            LIBS += -LC:/openCV/build/x86/mingw/lib
        }
        else: {
            message("Unsupported Windows Compiler or Environment")
            error("Use MinGW G++ or Visual C++ Compiler")
        }
    }

    CONFIG(release, debug|release): {
        LIBS += -lopencv_core249 \
                -lopencv_imgproc249 \
                -lopencv_highgui249 \
                -lopencv_ml249 \
                -lopencv_video249 \
                -lopencv_features2d249 \
                -lopencv_calib3d249 \
                -lopencv_objdetect249 \
                -lopencv_contrib249 \
                -lopencv_legacy249 \
                -lopencv_flann249
    } else: CONFIG(debug, debug|release): {
        LIBS += -lopencv_core249d \
                -lopencv_imgproc249d \
                -lopencv_highgui249d \
                -lopencv_ml249d \
                -lopencv_video249d \
                -lopencv_features2d249d \
                -lopencv_calib3d249d \
                -lopencv_objdetect249d \
                -lopencv_contrib249d \
                -lopencv_legacy249d \
                -lopencv_flann249d
    }
} else:unix: {
    CONFIG    += link_pkgconfig
    PKGCONFIG += opencv
} else: {
    message("Unsupported Platform")
    error("Supported Platforms are Windows and UNIX.")
}
