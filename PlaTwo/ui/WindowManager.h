#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H
#include <QObject>
#include <QMap>
#include "LoginWindow.h"
#include "SignupWindow.h"
#include "ForgotPasswordWindow.h"
#include "MainMenuWindow.h"
// #include "GameRoomWindow.h"
// #include "ProfileEditorWindow.h"
#include "../auth/AuthManager.h"
class WindowManager : public QObject
{
    Q_OBJECT
public:
    enum WindowType {
        Login,
        Signup,
        ForgotPassword,
        MainMenu,
        GameRoom,
        ProfileEditor
    };
private:
    // windows
    LoginWindow * loginWindow;
    SignupWindow * signupWindow;
    ForgotPasswordWindow* forgotPasswordWindow;
    MainMenuWindow* mainMenuWindow;
    // GameRoomWindow* gameRoomWindow;
    // ProfileEditorWindow* profileEditorWindow;

    // ManagePlayers
    AuthManager * authManager;
    QString currentPlayer;
    WindowType currentWindow;

    // helper_methods
    void createWindow(WindowType type);
    void cleanupWindow(WindowType type);
    void hideAllWindows();
    void setupConnections();
public:
    explicit WindowManager(QObject * parent = nullptr);
    ~WindowManager();

    // show_windows
    void showWindow(WindowType type);
    void showWindow(WindowType type, const QString& gameType);
    void hideWindow(WindowType type);
    void closeWindow(WindowType type);

    // Manage_moves
    void switchToLogin();
    void switchToSignup();
    void switchToForgotPassword();
    void switchToMainMenu(const QString& username);
    void switchToGameRoom(const QString& gameType, const QString& username);
    void switchToProfileEditor(const QString& username);

    // getters
    LoginWindow * getLoginWindow() const {return loginWindow;}
    MainMenuWindow * getMainMenuWindow() const {return mainMenuWindow;}

    // manager_players
    void setCurrentPlayer(const QString& username);
    QString getCurrentPlayer() const {return currentPlayer;}
    AuthManager* getAuthManager() const {return authManager;}
signals:
    void windowShown(WindowType type);
    void windowHidden(WindowType type);
    void windowClosed(WindowType type);
    void playerLoggedIn(const QString& username);
    void playerLoggedOut();
private slots:
    void onLoginSuccess(const QString& username);
    void onLoginFailed(const QString& reason);
    void onLogout();
    void onSignupCompleted(const QString& username);
    void onGameRoomClosed();
    void onProfileEditorClosed();

};

#endif // WINDOWMANAGER_H
