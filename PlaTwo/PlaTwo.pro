QT += core widgets network
CONFIG += c++17
TARGET = MyGameProject
TEMPLATE = app
SOURCES += \
    MainMenuWindow.cpp \
    SignupWindow.cpp \
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
    ui/ForgotPasswordWindow.cpp \
    ui/GameLobbyWindow.cpp \
    ui/LoginWindow.cpp \
    ui/ProfileEditorWindow.cpp \
    ui/WindowManager.cpp \
    utils/*.cpp \
    ui/*.cpp
HEADERS += \
    MainMenuWindow.h \
    SignupWindow.h \
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
    ui/ForgotPasswordWindow.h \
    ui/GameLobbyWindow.h \
    ui/LoginWindow.h \
    ui/ProfileEditorWindow.h \
    ui/WindowManager.h \
    utils/*.h \
    ui/*.h
FORMS += \
    mainwindow.ui \
    ui/forms/*.ui
RESOURCES += resources/resources.qrc
INCLUDEPATH += $$PWD
