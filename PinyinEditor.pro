QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PinyinEditor
TEMPLATE = app


SOURCES += src/main.cpp \
    src/pinyin_convert.cpp \
    src/pinyin_editor.cpp

HEADERS  += src/common.h \
    src/static_map.h \
    src/pinyin_editor.h

FORMS    += ui/pinyin_editor.ui
