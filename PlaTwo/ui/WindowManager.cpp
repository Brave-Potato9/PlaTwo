#include "WindowManager.h"
#include <QMessageBox>
#include <QFileInfo>
WindowManager::WindowManager(QObject *parent)
    : QObject(parent)
    , loginWindow(nullptr)
    , signupWindow(nullptr)
    , forgotPasswordWindow(nullptr)
    , mainMenuWindow(nullptr)
    , gameLobbyWindow(nullptr)
    , profileEditorWindow(nullptr)
    , authManager(new AuthManager(this))
    , networkManager(new NetworkManager(this))
    , currentWindow(Login)
    , isServerMode(false)
    , serverPort(12345)
{
    setupConnections();
    setupNetworkConnections();
    createWindow(Login);
    loginWindow->show();
    currentWindow = Login;
}
WindowManager::~WindowManager() {
    closeAllGameRooms();
    cleanupWindow(Login);
    cleanupWindow(Signup);
    cleanupWindow(ForgotPassword);
    cleanupWindow(MainMenu);
    cleanupWindow(GameLobby);
    cleanupWindow(ProfileEditor);
}
void WindowManager::setupConnections() {
    connect(authManager, &AuthManager::loginSuccess, this, &WindowManager::onLoginSuccess);
    connect(authManager, &AuthManager::loginFailed, this, &WindowManager::onLoginFailed);
}
void WindowManager::setupNetworkConnections()
{
    if (!networkManager) return;

    // server
    connect(networkManager, &NetworkManager::roomCreated, this, &WindowManager::onRoomCreated);
    connect(networkManager, &NetworkManager::roomDestroyed, this, &WindowManager::onRoomDestroyed);
    connect(networkManager, &NetworkManager::clientConnected, this, &WindowManager::onClientConnected);
    connect(networkManager, &NetworkManager::clientDisconnected, this, &WindowManager::onClientDisconnected);
    // client
    connect(networkManager, &NetworkManager::connectedToServer, this, &WindowManager::onConnectedToServer);
    connect(networkManager, &NetworkManager::disconnectedFromServer, this, &WindowManager::onDisconnectedFromServer);
    connect(networkManager, &NetworkManager::connectionFailed, this, &WindowManager::onConnectionFailed);
    connect(networkManager, &NetworkManager::joinedRoom, this, &WindowManager::onJoinedRoom);
    connect(networkManager, &NetworkManager::joinFailed, this, &WindowManager::onJoinFailed);
    connect(networkManager, &NetworkManager::gameStarted, this, &WindowManager::onGameStartedNetwork);
    connect(networkManager, &NetworkManager::gameEnded, this, &WindowManager::onGameEndedNetwork);
}
void WindowManager::createWindow(WindowType type) {
    switch(type) {
    case Login:
        if(!loginWindow) {
            loginWindow = new LoginWindow(authManager, nullptr);
            connect(loginWindow, &LoginWindow::signupRequested, this, &WindowManager::switchToSignup);
            connect(loginWindow, &LoginWindow::forgotPasswordRequested, this, &WindowManager::switchToForgotPassword);
            connect(loginWindow, &LoginWindow::loginSuccess, this, &WindowManager::onLoginSuccess);

        }
        break;
    case Signup:
        if(!signupWindow) {
            signupWindow = new SignupWindow(authManager, nullptr);
            connect(signupWindow, &SignupWindow::signupCompleted, this, &WindowManager::onSignupCompleted);
            connect(signupWindow, &SignupWindow::canceled, this, &WindowManager::switchToLogin);
        }
        break;
    case ForgotPassword:
        if(!forgotPasswordWindow) {
            forgotPasswordWindow = new ForgotPasswordWindow(authManager, nullptr);
            connect(forgotPasswordWindow, &ForgotPasswordWindow::passwordReset, this, &WindowManager::switchToLogin);
            connect(forgotPasswordWindow, &ForgotPasswordWindow::canceled, this, &WindowManager::switchToLogin);
        }
        break;
    case MainMenu:
        if(!mainMenuWindow) {
            mainMenuWindow = new MainMenuWindow(authManager, currentPlayer, nullptr);
            connect(mainMenuWindow, &MainMenuWindow::logoutRequested, this, &WindowManager::onLogout);
            connect(mainMenuWindow, &MainMenuWindow::gameSelected, this, &WindowManager::onGameSelected);
        }
     break;
    case GameRoom:

     break;
    case GameLobby:
        break;
    case ProfileEditor:
    if(!profileEditorWindow) {
       profileEditorWindow = new ProfileEditorWindow(authManager, currentPlayer, nullptr);
        connect(profileEditorWindow, &ProfileEditorWindow::closed, this, &WindowManager::onProfileEditorClosed);
        connect(profileEditorWindow, &ProfileEditorWindow::profileUpdated, this, &WindowManager::switchToForgotPassword);
    }
     break;
    }
}
void WindowManager::cleanupWindow(WindowType type)
{
    switch(type) {
    case Login:
        if (loginWindow) {
            loginWindow->close();
            loginWindow->deleteLater();
            loginWindow = nullptr;
        }
        break;
    case Signup:
        if (signupWindow) {
            signupWindow->close();
            signupWindow->deleteLater();
            signupWindow = nullptr;
        }
        break;
    case ForgotPassword:
        if (forgotPasswordWindow) {
            forgotPasswordWindow->close();
            forgotPasswordWindow->deleteLater();
            forgotPasswordWindow = nullptr;
        }
        break;
    case MainMenu:
        if (mainMenuWindow) {
            mainMenuWindow->close();
            mainMenuWindow->deleteLater();
            mainMenuWindow = nullptr;
        }
        break;
    case GameLobby:
        if (gameLobbyWindow) {
            gameLobbyWindow->close();
            gameLobbyWindow->deleteLater();
            gameLobbyWindow = nullptr;
        }
        break;
    case GameRoom:
        break;
    case ProfileEditor:
        if (profileEditorWindow) {
            profileEditorWindow->close();
            profileEditorWindow->deleteLater();
            profileEditorWindow = nullptr;
        }
        break;
    }
}
void WindowManager::showWindow(WindowType type) {
    createWindow(type);
    hideAllWindows();

    switch(type) {
    case Login:
        if(loginWindow) {
            loginWindow->show();
            loginWindow->raise();
            loginWindow->activateWindow();
        }
        break;
    case Signup:
        if(signupWindow) {
            signupWindow->show();
            signupWindow->raise();
            signupWindow->activateWindow();
        }
        break;
    case ForgotPassword:
        if(forgotPasswordWindow) {
            forgotPasswordWindow->show();
            forgotPasswordWindow->raise();
            forgotPasswordWindow->activateWindow();
        }
        break;
    case MainMenu:
        if(mainMenuWindow) {
            mainMenuWindow->show();
            mainMenuWindow->raise();
            mainMenuWindow->activateWindow();
        }
        break;
    case GameLobby:
        break;
    case GameRoom:
        break;
    case ProfileEditor:
        if (profileEditorWindow) {
            profileEditorWindow->show();
            profileEditorWindow->raise();
            profileEditorWindow->activateWindow();
        }
        break;
    }
    currentWindow = type;
    emit windowShown(type);

}
void WindowManager::showWindow(WindowType type, const QString& gameType) {
    if (type == GameLobby) {
        if (gameLobbyWindow) {
            gameLobbyWindow->close();
            gameLobbyWindow->deleteLater();
            gameLobbyWindow = nullptr;
        }
        gameLobbyWindow = new GameLobbyWindow(gameType, authManager, currentPlayer, nullptr);
        connect(gameLobbyWindow, &GameLobbyWindow::gameStarted, this, &WindowManager::onGameStarted);
        connect(gameLobbyWindow, &GameLobbyWindow::gameLoaded, this, &WindowManager::onGameLoaded);
        connect(gameLobbyWindow, &GameLobbyWindow::backToMenu, this, &WindowManager::onBackToMenu);
        connect(gameLobbyWindow, &GameLobbyWindow::joinGameRequested, this, &WindowManager::onJoinGameRequested);

        hideAllWindows();
        gameLobbyWindow->show();
        gameLobbyWindow->raise();
        gameLobbyWindow->activateWindow();
        currentWindow = GameLobby;
        emit windowShown(GameLobby);
    } else {
        showWindow(type);
    }

}
void WindowManager::hideAllWindows() {
    if (loginWindow) loginWindow->hide();
    if (signupWindow) signupWindow->hide();
    if (forgotPasswordWindow) forgotPasswordWindow->hide();
    if (mainMenuWindow) mainMenuWindow->hide();
    if (gameLobbyWindow) gameLobbyWindow->hide();
    if (profileEditorWindow) profileEditorWindow->hide();
}
void WindowManager::hideWindow(WindowType type) {
    switch(type) {
    case Login:
        if (loginWindow) loginWindow->hide();
        break;
    case Signup:
        if (signupWindow) signupWindow->hide();
        break;
    case ForgotPassword:
        if (forgotPasswordWindow) forgotPasswordWindow->hide();
        break;
    case MainMenu:
        if (mainMenuWindow) mainMenuWindow->hide();
        break;
    case GameLobby:
        if (gameLobbyWindow) gameLobbyWindow->hide();
        break;
    case GameRoom:
        break;
    case ProfileEditor:
        if (profileEditorWindow) profileEditorWindow->hide();
        break;
    }
}
void WindowManager::showWindow(WindowType type, const QString& gameType, const GameConfig& config)
{
    if (type == GameRoom) {
        Q_UNUSED(gameType);
        Q_UNUSED(config);
    } else {
        showWindow(type, gameType);
    }
}
void WindowManager::closeWindow(WindowType type) {
    switch (type) {
    case Login:
        cleanupWindow(Login);
        break;
    case Signup:
        cleanupWindow(Signup);
        break;
    case ForgotPassword:
        cleanupWindow(ForgotPassword);
        break;
    case MainMenu:
        cleanupWindow(MainMenu);
        break;
    case GameLobby:
        cleanupWindow(GameLobby);
        break;
    case GameRoom:
        closeAllGameRooms();
        break;
    case ProfileEditor:
        cleanupWindow(ProfileEditor);
        break;
    }
}
void WindowManager::closeAllGameRooms() {
    for (auto it = gameRoomWindows.begin(); it != gameRoomWindows.end(); ++it) {
        if (it.value()) {
            it.value()->close();
            it.value()->deleteLater();
        }
    }
    gameRoomWindows.clear();
    roomGameTypes.clear();
}
QString WindowManager::generateRoomId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}
void WindowManager::switchToLogin() {
    showWindow(Login);
}
void WindowManager::switchToSignup() {
    showWindow(Signup);
}
void WindowManager::switchToForgotPassword() {
    showWindow(ForgotPassword);
}
void WindowManager::switchToMainMenu(const QString& username) {
    currentPlayer = username;
    if(mainMenuWindow) {
        mainMenuWindow->setUsername(username);
    }
    showWindow(MainMenu);
}
void WindowManager::switchToGameLobby(const QString& gameType, const QString& username)
{
    currentPlayer = username;
    showWindow(GameLobby, gameType);
}
void WindowManager::switchToGameRoom(const QString& roomId, const QString& gameType, const GameConfig& config, const QString& username,
                                     const QColor& playerColor, bool isHost) {
    currentPlayer = username;

    if (isHost) {
        createGameRoom(roomId, gameType, config, username, playerColor, true);
    } else {
        joinGameRoom(roomId, gameType, config, username, playerColor);
    }
}
void WindowManager::switchToProfileEditor(const QString& username) {
    currentPlayer = username;
    if (profileEditorWindow) {
        profileEditorWindow->deleteLater();
        profileEditorWindow = nullptr;
    }
    profileEditorWindow = new ProfileEditorWindow(authManager, currentPlayer, nullptr);
    connect(profileEditorWindow, &ProfileEditorWindow::closed, this, &WindowManager::onProfileEditorClosed);
    connect(profileEditorWindow, &ProfileEditorWindow::profileUpdated, this, &WindowManager::switchToMainMenu);
    showWindow(ProfileEditor);
}
void WindowManager::createGameRoom(const QString& roomId, const QString& gameType, const GameConfig& config, const QString& username,
                                   const QColor& playerColor, bool isHost) {
    if (gameRoomWindows.contains(roomId)) {
        gameRoomWindows[roomId]->close();
        gameRoomWindows[roomId]->deleteLater();
        gameRoomWindows.remove(roomId);
    }

    Room* room = nullptr;
    if (networkManager && networkManager->isServerRunning()) {
        room = networkManager->getServer()->getRoom(roomId);
    }

    if (!room && isHost) {
        if (networkManager && networkManager->isServerRunning()) {
            networkManager->createRoom(roomId, config, username);
            room = networkManager->getServer()->getRoom(roomId);
        }
    }

    GameRoomWindow* roomWindow = new GameRoomWindow(
        room,
        gameType,
        config,
        authManager,
        username,
        playerColor,
        networkManager,
        isHost,
        roomId,
        nullptr
        );

    gameRoomWindows[roomId] = roomWindow;
    roomGameTypes[roomId] = gameType;

    connect(roomWindow, &GameRoomWindow::closed, this, &WindowManager::onGameRoomClosed);
    connect(roomWindow, &GameRoomWindow::gameFinished, this, &WindowManager::onGameFinished);

    hideAllWindows();
    roomWindow->show();
    roomWindow->raise();
    roomWindow->activateWindow();
    currentWindow = GameRoom;
    emit windowShown(GameRoom);
    emit gameRoomCreated(roomId);
}
void WindowManager::joinGameRoom(const QString& roomId, const QString& gameType, const GameConfig& config, const QString& username, const QColor& playerColor) {
    if (networkManager) {
        networkManager->joinRoom(roomId, username);
    }

    createGameRoom(roomId, gameType, config, username, playerColor, false);
}

void WindowManager::leaveCurrentRoom()
{
    if (networkManager) {
        networkManager->leaveRoom();
    }

    QString currentRoomId = getCurrentRoomId();
    if (!currentRoomId.isEmpty()) {
        if (gameRoomWindows.contains(currentRoomId)) {
            gameRoomWindows[currentRoomId]->close();
            gameRoomWindows[currentRoomId]->deleteLater();
            gameRoomWindows.remove(currentRoomId);
            roomGameTypes.remove(currentRoomId);
        }
        emit gameRoomClosed(currentRoomId);
    }

    switchToMainMenu(currentPlayer);
}
bool WindowManager::hasActiveGameRoom() const
{
    return !gameRoomWindows.isEmpty();
}

QString WindowManager::getCurrentRoomId() const
{
    if (gameRoomWindows.isEmpty()) return "";
    return gameRoomWindows.firstKey();
}

bool WindowManager::startServer(int port)
{
    serverPort = port;
    isServerMode = networkManager->startServer(port);

    if (isServerMode) {
        qDebug() << "Server started on port" << port;
        qDebug() << "Server IP:" << networkManager->getServerIP();
    }

    return isServerMode;
}

void WindowManager::stopServer()
{
    networkManager->stopServer();
    isServerMode = false;
    qDebug() << "Server stopped";
}
bool WindowManager::isServerRunning() const {
    return networkManager->isServerRunning();
}

QString WindowManager::getServerIP() const {
    return networkManager->getServerIP();
}

int WindowManager::getServerPort() const {
    return serverPort;
}

void WindowManager::onLoginSuccess(const QString& username)
{
    currentPlayer = username;
    emit playerLoggedIn(username);
    switchToMainMenu(username);
}

void WindowManager::onLoginFailed(const QString& reason)
{
    QMessageBox::warning(nullptr, "Login Failed", reason);
}
void WindowManager::onLogout() {
    authManager->logout(currentPlayer);

    closeAllGameRooms();
    if (networkManager) {
        networkManager->disconnectFromServer();
        if (networkManager->isServerRunning()) {
            networkManager->stopServer();
        }
    }

    currentPlayer.clear();
    emit playerLoggedOut();
    switchToLogin();
}
void WindowManager::onSignupCompleted(const QString& username) {
    Q_UNUSED(username);
    switchToLogin();
}

void WindowManager::onGameSelected(const QString& gameType, const QString& username) {
    switchToGameLobby(gameType, username);
}

void WindowManager::onProfileEditRequested(const QString& username) {
    switchToProfileEditor(username);
}

void WindowManager::onGameStarted(const GameConfig& config, const QColor& playerColor) {
    QString gameType = config.getGameType();
    QString roomId = generateRoomId();

    if (!networkManager->isServerRunning()) {
        int port = config.getServerPort();
        if (port <= 0) port = 12345;
        startServer(port);
    }

    switchToGameRoom(roomId, gameType, config, currentPlayer, playerColor, true);
}
void WindowManager::onGameLoaded(const QString& saveFile, const QColor& playerColor) {
    QString gameType = QFileInfo(saveFile).baseName().split("_").first();
    GameConfig config;
    config.setGameType(gameType);

    QString roomId = generateRoomId();

    if (!networkManager->isServerRunning()) {
        startServer(12345);
    }

    switchToGameRoom(roomId, gameType, config, currentPlayer, playerColor, true);
}
void WindowManager::onJoinGameRequested(const QString& ip, int port, const QColor& playerColor) {
    if (networkManager->connectToServer(ip, port)) {
        QString roomId = "";
        roomId = "game_room";

        GameConfig config;
        config.setGameType(gameLobbyWindow ? "DotsAndBoxes" : "Unknown");
        config.setServerIP(ip);
        config.setServerPort(port);

        switchToGameRoom(roomId, config.getGameType(), config, currentPlayer, playerColor, false);
    } else {
        QMessageBox::warning(nullptr, "Connection Failed",
                             "Could not connect to server at " + ip + ":" + QString::number(port));
    }
}
void WindowManager::onBackToMenu() {
    switchToMainMenu(currentPlayer);
}

void WindowManager::onGameRoomClosed()
{
    GameRoomWindow* roomWindow = qobject_cast<GameRoomWindow*>(sender());
    if (!roomWindow) return;

    QString roomId;
    for (auto it = gameRoomWindows.begin(); it != gameRoomWindows.end(); ++it) {
        if (it.value() == roomWindow) {
            roomId = it.key();
            break;
        }
    }

    if (!roomId.isEmpty()) {
        gameRoomWindows.remove(roomId);
        roomGameTypes.remove(roomId);
        emit gameRoomClosed(roomId);
    }

    if (!gameRoomWindows.isEmpty()) {
        QString lastRoomId = gameRoomWindows.lastKey();
        if (gameRoomWindows[lastRoomId]) {
            gameRoomWindows[lastRoomId]->show();
            gameRoomWindows[lastRoomId]->raise();
            gameRoomWindows[lastRoomId]->activateWindow();
        }
    } else {
        switchToMainMenu(currentPlayer);
    }
}
void WindowManager::onProfileEditorClosed() {
    if (profileEditorWindow) {
        profileEditorWindow->deleteLater();
        profileEditorWindow = nullptr;
    }
    switchToMainMenu(currentPlayer);
}

void WindowManager::onGameFinished(const QString& winner) {
    qDebug() << "Game finished. Winner:" << winner;
}

void WindowManager::onRoomCreated(const QString& roomId) {
    qDebug() << "Room created:" << roomId;
}

void WindowManager::onRoomDestroyed(const QString& roomId) {
    qDebug() << "Room destroyed:" << roomId;

    if (gameRoomWindows.contains(roomId)) {
        gameRoomWindows[roomId]->close();
        gameRoomWindows[roomId]->deleteLater();
        gameRoomWindows.remove(roomId);
        roomGameTypes.remove(roomId);
        emit gameRoomClosed(roomId);
    }
}

void WindowManager::onJoinedRoom(const QString& roomId) {
    qDebug() << "Joined room:" << roomId;
}

void WindowManager::onJoinFailed(const QString& reason) {
    QMessageBox::warning(nullptr, "Join Failed",
                         "Could not join room:\n" + reason);
    switchToMainMenu(currentPlayer);
}

void WindowManager::onGameStartedNetwork(const QString& roomId) {
    qDebug() << "Game started in room:" << roomId;
}
void WindowManager::onGameEndedNetwork(const QString& roomId, const QString& winner) {
    qDebug() << "Game ended in room:" << roomId << "Winner:" << winner;
}

void WindowManager::onClientConnected(const QString& username) {
    qDebug() << "Client connected:" << username;
}

void WindowManager::onClientDisconnected(const QString& username) {
    qDebug() << "Client disconnected:" << username;
}
void WindowManager::onConnectedToServer()
{
    qDebug() << "Connected to server";
    gameRoomWindows[getCurrentRoomId()]->updateStatusBar("Connected to server", "#27ae60");
}

void WindowManager::onDisconnectedFromServer()
{
    qDebug() << "Disconnected from server";
    gameRoomWindows[getCurrentRoomId()]->updateStatusBar("Disconnected from server", "#e74c3c");
}
void WindowManager::onConnectionFailed(const QString& reason)
{
    qDebug() << "Connection failed:" << reason;
    QMessageBox::warning(nullptr, "Connection Failed", reason);
}
void WindowManager::setCurrentPlayer(const QString& username) {
    currentPlayer = username;
}