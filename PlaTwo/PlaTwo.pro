QT += core widgets network
CONFIG += c++17
TARGET = MyGameProject
TEMPLATE = app
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    core/*.cpp \
    auth/*.cpp \
    network/*.cpp \
    games/dotsandboxes/*.cpp \
    games/morris/*.cpp \
    games/fanorona/*.cpp \
    network/Client.cpp \
    network/NetworkManager.cpp \
    network/Room.cpp \
    network/Server.cpp \
    utils/*.cpp \
    ui/*.cpp
HEADERS += \
    mainwindow.h \
    core/*.h \
    auth/*.h \
    network/*.h \
    games/dotsandboxes/*.h \
    games/morris/*.h \
    games/fanorona/*.h \
    network/Client.h \
    network/NetworkManager.h \
    network/Room.h \
    network/Server.h \
    utils/*.h \
    ui/*.h
FORMS += \
    mainwindow.ui \
    ui/forms/*.ui
RESOURCES += resources/resources.qrc
INCLUDEPATH += $$PWD
