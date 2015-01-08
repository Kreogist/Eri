QT += core\
      gui\
      widgets

CONFIG += c++11

INCLUDEPATH += sdk

#FFMpeg libs
LIBS += -lavformat -lavcodec -lavutil
LIBS += -lswresample
LIBS += -lOpenAL32

SOURCES += \
    main.cpp \
    decoder/kedecoderffmpeg/keffmpegglobal.cpp \
    sdk/kedecoderbase.cpp \
    decoder/kedecoderffmpeg/kedecoderffmpeg.cpp \
    sdk/keglobal.cpp

HEADERS += \
    decoder/kedecoderffmpeg/keffmpegglobal.h \
    sdk/kedecoderbase.h \
    decoder/kedecoderffmpeg/kedecoderffmpeg.h \
    sdk/keglobal.h
