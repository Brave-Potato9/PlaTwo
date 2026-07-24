#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QMap>
#include <QUuid>
#include "LoginWindow.h"
#include "SignupWindow.h"
#include "ForgotPasswordWindow.h"
#include "MainMenuWindow.h"
#include "GameLobbyWindow.h"
#include "GameRoomWindow.h"
#include "ProfileEditorWindow.h"
#include "../auth/AuthManager.h"
#include "../network/NetworkManager.h"
#include "../network/Room.h"

class WindowManager : public QObject
{
    Q_OBJECT

public:
    enum WindowType {
        Login,
        Signup,
        ForgotPassword,
        MainMenu,
        GameLobby,
        GameRoom,
        ProfileEditor
    };

private:
    // windows
    LoginWindow* loginWindow;
    SignupWindow* signupWindow;
    ForgotPasswordWindow* forgotPasswordWindow;
    MainMenuWindow* mainMenuWindow;
    GameLobbyWindow* gameLobbyWindow;
    ProfileEditorWindow* profileEditorWindow;

    // game rooms maanger
    QMap<QString, GameRoomWindow*> gameRoomWindows;
    QMap<QString, QString> roomGameTypes; // roomId -> gameType

    // managers
    AuthManager* authManager;
    NetworkManager* networkManager;
    QString currentPlayer;
    WindowType currentWindow;

    // status
    bool isServerMode;
    int serverPort;

    // helper methods
    void createWindow(WindowType type);
    void cleanupWindow(WindowType type);
    void hideAllWindows();
    void hideWindow(WindowType type);
    void setupConnections();
    void setupNetworkConnections();
    QString generateRoomId() const;
    void initializeNetwork();

public:
    explicit WindowManager(QObject *parent = nullptr);
    ~WindowManager();

    // show windows
    void showWindow(WindowType type);
    void showWindow(WindowType type, const QString& gameType);
    void showWindow(WindowType type, const QString& gameType, const GameConfig& config);
    void closeWindow(WindowType type);
    void closeAllGameRooms();

    // move between windows
    void switchToLogin();
    void switchToSignup();
    void switchToForgotPassword();
    void switchToMainMenu(const QString& username);
    void switchToGameLobby(const QString& gameType, const QString& username);
    void switchToGameRoom(const QString& roomId, const QString& gameType,
                          const GameConfig& config, const QString& username,
                          const QColor& playerColor, bool isHost);
    void switchToProfileEditor(const QString& username);

    // room manager
    void createGameRoom(const QString& roomId, const QString& gameType,
                        const GameConfig& config, const QString& username,
                        const QColor& playerColor, bool isHost);
    void joinGameRoom(const QString& roomId, const QString& gameType,
                      const GameConfig& config, const QString& username,
                      const QColor& playerColor);
    void leaveCurrentRoom();
    bool hasActiveGameRoom() const;
    QString getCurrentRoomId() const;

    // server manager
    bool startServer(int port = 12345);
    void stopServer();
    bool isServerRunning() const;
    QString getServerIP() const;
    int getServerPort() const;

    // player manager
    void setCurrentPlayer(const QString& username);
    QString getCurrentPlayer() const { return currentPlayer; }
    AuthManager* getAuthManager() const { return authManager; }
    NetworkManager* getNetworkManager() const { return networkManager; }

signals:
    void windowShown(WindowType type);
    void windowHidden(WindowType type);
    void windowClosed(WindowType type);
    void playerLoggedIn(const QString& username);
    void playerLoggedOut();
    void gameRoomCreated(const QString& roomId);
    void gameRoomClosed(const QString& roomId);

private slots:
    // auth
    void onLoginSuccess(const QString& username);
    void onLoginFailed(const QString& reason);
    void onLogout();
    void onSignupCompleted(const QString& username);

    // menu and lobby
    void onGameSelected(const QString& gameType, const QString& username);
    void onProfileEditRequested(const QString& username);
    void onGameStarted(const GameConfig& config, const QColor& playerColor);
    void onGameLoaded(const QString& saveFile, const QColor& playerColor);
    void onJoinGameRequested(const QString& ip, int port, const QColor& playerColor);
    void onBackToMenu();

    // GameRoom
    void onGameRoomClosed();
    void onProfileEditorClosed();
    void onGameFinished(const QString& winner);

    // network
    void onRoomCreated(const QString& roomId);
    void onRoomDestroyed(const QString& roomId);
    void onJoinedRoom(const QString& roomId);
    void onJoinFailed(const QString& reason);
    void onGameStartedNetwork(const QString& roomId);
    void onGameEndedNetwork(const QString& roomId, const QString& winner);
    void onClientConnected(const QString& username);
    void onClientDisconnected(const QString& username);
    void onConnectedToServer();
    void onDisconnectedFromServer();
    void onConnectionFailed(const QString& reason);
};

#endif // WINDOWMANAGER_H