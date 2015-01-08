QT += core\
      gui\
      widgets

CONFIG += c++11 libFFMpeg libPortAudio

INCLUDEPATH += sdk

#FFMpeg configure
libFFMpeg{
    LIBS += -lavformat -lavcodec -lavutil
    LIBS += -lswresample
    SOURCES += decoder/kedecoderffmpeg/keffmpegglobal.cpp \
               decoder/kedecoderffmpeg/kedecoderffmpeg.cpp
    HEADERS += decoder/kedecoderffmpeg/keffmpegglobal.h \
               decoder/kedecoderffmpeg/kedecoderffmpeg.h
}

libPortAudio{
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
    sdk/knconnectionhandler.cpp

HEADERS += \
    sdk/keglobal.h \
    sdk/kedecoderbase.h \
    sdk/keplaybackbase.h \
    sdk/knconnectionhandler.h
