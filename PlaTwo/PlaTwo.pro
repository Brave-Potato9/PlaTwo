QT += core widgets network

CONFIG += c++17

TARGET = PlaTwo
TEMPLATE = app

# مسیرهای include
INCLUDEPATH += $$PWD/auth \
               $$PWD/core \
               $$PWD/network \
               $$PWD/utils \
               $$PWD/games/dotsandboxes \
               $$PWD/games/morris \
               $$PWD/games/fanorona \
               $$PWD/ui
SOURCES += \
    main.cpp \
    auth/AuthManager.cpp \
    auth/PasswordHasher.cpp \
    auth/PlayerManager.cpp \
    core/Game.cpp \
    core/GameHistory.cpp \
    core/GameHistoryManager.cpp \
    core/GameSession.cpp \
    core/Move.cpp \
    core/Player.cpp \
    network/Client.cpp \
    network/NetworkManager.cpp \
    network/Room.cpp \
    network/Server.cpp \
    utils/GameConfig.cpp \
    utils/TimerManager.cpp \
    utils/Validator.cpp \
    games/dotsandboxes/DotsAndBoxesBoard.cpp \
    games/dotsandboxes/DotsAndBoxesGame.cpp \
    games/morris/MorrisBoard.cpp \
    games/morris/MorrisGame.cpp \
    games/fanorona/FanoronaBoard.cpp \
    games/fanorona/FanoronaGame.cpp \
    ui/LoginWindow.cpp \
    ui/SignupWindow.cpp \
    ui/ForgotPasswordWindow.cpp \
    ui/MainMenuWindow.cpp \
    ui/GameLobbyWindow.cpp \
    ui/GameRoomWindow.cpp \
    ui/ProfileEditorWindow.cpp \
    ui/WindowManager.cpp \
    ui/GameBoardWidget.cpp

# فایل‌های هدر
HEADERS += \
    auth/AuthManager.h \
    auth/PasswordHasher.h \
    auth/PlayerManager.h \
    core/Game.h \
    core/GameHistory.h \
    core/GameHistoryManager.h \
    core/GameSession.h \
    core/Move.h \
    core/Player.h \
    network/Client.h \
    network/NetworkManager.h \
    network/Room.h \
    network/Server.h \
    utils/GameConfig.h \
    utils/TimerManager.h \
    utils/Validator.h \
    games/dotsandboxes/DotsAndBoxesBoard.h \
    games/dotsandboxes/DotsAndBoxesGame.h \
    games/morris/MorrisBoard.h \
    games/morris/MorrisGame.h \
    games/fanorona/FanoronaBoard.h \
    games/fanorona/FanoronaGame.h \
    ui/LoginWindow.h \
    ui/SignupWindow.h \
    ui/ForgotPasswordWindow.h \
    ui/MainMenuWindow.h \
    ui/GameLobbyWindow.h \
    ui/GameRoomWindow.h \
    ui/ProfileEditorWindow.h \
    ui/WindowManager.h \
    ui/GameBoardWidget.h

FORMS += \
    ui/forms/LoginWindow.ui \
    ui/forms/SignupWindow.ui \
    ui/forms/ForgotPasswordWindow.ui \
    ui/forms/MainMenuWindow.ui \
    ui/forms/GameLobbyWindow.ui \
    ui/forms/GameRoomWindow.ui \
    ui/forms/ProfileEditorWindow.ui

RESOURCES += \
    resources/resources.qrc

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/bin/debug
} else {
    DESTDIR = $$PWD/bin/release
}