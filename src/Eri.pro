QT += core\
      gui\
      widgets

CONFIG += c++11 libFFMpeg libPortAudio

INCLUDEPATH += sdk

#FFMpeg configure
libFFMpeg{
    macx: {
        #Use brew to install ffmpeg.
        LIBS += -L/usr/local/lib/
        LIBS += -framework CoreFoundation
    }
    LIBS += -lavformat -lavcodec -lavutil -lswresample -lswscale
    SOURCES += decoder/kedecoderffmpeg/keffmpegglobal.cpp \
               decoder/kedecoderffmpeg/kedecoderffmpeg.cpp
    HEADERS += decoder/kedecoderffmpeg/keffmpegglobal.h \
               decoder/kedecoderffmpeg/kedecoderffmpeg.h
}

libPortAudio{
    DEFINES += ENABLE_PORTAUDIO
    macx: {
        #Use brew to install PortAudio.
        LIBS += -L/usr/local/lib/
        INCLUDEPATH += /usr/local/include/
    }
    LIBS += -lportaudio
    SOURCES += playback/keplaybackportaudio/keplaybackportaudio.cpp \
               playback/keplaybackportaudio/keportaudioglobal.cpp
    HEADERS += playback/keplaybackportaudio/keplaybackportaudio.h \
               playback/keplaybackportaudio/keportaudioglobal.h
}

SOURCES += \
    main.cpp \
    sdk/keglobal.cpp \
    sdk/kedecoderbase.cpp \
    sdk/keplaybackbase.cpp \
    sdk/knconnectionhandler.cpp \
    mainwindow.cpp \
    sdk/keplayer.cpp

HEADERS += \
    sdk/keglobal.h \
    sdk/kedecoderbase.h \
    sdk/keplaybackbase.h \
    sdk/knconnectionhandler.h \
    mainwindow.h \
    sdk/keplayer.h
