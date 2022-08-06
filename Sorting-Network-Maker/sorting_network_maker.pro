QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sorting-Network-Maker

TEMPLATE = app

SOURCES += src/main.cpp \
           src/sorting_network_maker.cpp \
           src/sorting_network_generator.cpp \
           src/sorting_network_painter.cpp \
           src/sorting_network_tester.cpp

HEADERS  += src/sorting_network_maker.h \
            src/generators.h \
            src/tester.h

FORMS    += ui/sorting_network_maker.ui

RESOURCES += rsrc/sorting_network_maker.qrc
