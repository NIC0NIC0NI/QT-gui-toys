QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pinyin-Bianjiqi

TEMPLATE = app

SOURCES += src/main.cpp \
    src/pinyin_convert.cpp \
    src/pinyin_bianjiqi.cpp

HEADERS  += src/common.h \
     src/static_map.h \
     src/pinyin_bianjiqi.h

FORMS    += ui/pinyin_bianjiqi.ui

RESOURCES += rsrc/pinyin_bianjiqi.qrc

TRANSLATIONS += lang/cn.ts

# QMAKE_CXXFLAGS += -DQT_NO_CAST_FROM_ASCII
# RC_FILE = rsrc/icon.rc
