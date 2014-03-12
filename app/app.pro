include (../caiga.pri)

TEMPLATE = app

SOURCES += main.cpp

LIBS += -L../core \
        -L../gui \
        -lcore \
        -lgui
        
TARGET = ../caiga

windows: {
    RC_FILE      = winicon.rc
    OTHER_FILES += winicon.rc
    RESOURCES   += ../icons/icons.qrc
}
