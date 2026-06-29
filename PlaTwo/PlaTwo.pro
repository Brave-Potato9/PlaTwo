QT += core widgets network
CONFIG += c++17
TARGET = MyGameProject
TEMPLATE = app
SOURCES += \
    main.cpp \
    core/*.cpp \
    auth/*.cpp \
    network/*.cpp \
    games/dotsandboxes/*.cpp \
    games/morris/*.cpp \
    games/fanorona/*.cpp \
    utils/*.cpp \
    ui/*.cpp
HEADERS += \
    core/*.h \
    auth/*.h \
    network/*.h \
    games/dotsandboxes/*.h \
    games/morris/*.h \
    games/fanorona/*.h \
    utils/*.h \
    ui/*.h
FORMS += ui/forms/*.ui
RESOURCES += resources/resources.qrc
INCLUDEPATH += $$PWD