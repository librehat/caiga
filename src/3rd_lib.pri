windows: {
    #Change lines below according to your setup environment
    INCLUDEPATH += C:/openCV/build/include

    CONFIG(x64, x86|x64) {#pass argument "CONFIG+=x64" while building win64 target
        LIBS += -LC:/openCV/build/x64/vc11/lib
    } else {
        LIBS += -LC:/openCV/build/x86/mingw/lib
    }

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
} else:unix: {
    CONFIG    += link_pkgconfig
    PKGCONFIG += opencv
}
