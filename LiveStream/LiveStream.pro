QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Decode/VideoDecode.cpp \
    OpenGL/VideoPlayWidget.cpp \
    Shader/Shader.cpp \
    main.cpp \
    MainWidget.cpp

HEADERS += \
    Decode/VideoDecode.h \
    MainWidget.h \
    OpenGL/VideoPlayWidget.h \
    Shader/Shader.h \
    Shader/stb_image.h

FORMS += \
    MainWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += ../Include/FFmpeg
LIBS += -L../Lib/FFmpeg -lavcodec
LIBS += -L../Lib/FFmpeg -lavdevice
LIBS += -L../Lib/FFmpeg -lavfilter
LIBS += -L../Lib/FFmpeg -lavformat
LIBS += -L../Lib/FFmpeg -lavutil
LIBS += -L../Lib/FFmpeg -lpostproc
LIBS += -L../Lib/FFmpeg -lswresample
LIBS += -L../Lib/FFmpeg -lswscale

win32:LIBS += -lopengl32 \
                -lglu32

CONFIG(debug, debug|release){
    DESTDIR =$$PWD/../Debug
}else{
    DESTDIR =$$PWD/../Target
}
