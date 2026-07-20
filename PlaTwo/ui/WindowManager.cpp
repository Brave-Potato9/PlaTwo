#include "WindowManager.h"

WindowManager::WindowManager(QObject * parent):
    QObject(parent)
    ,loginWindow(nullptr)
    ,signupWindow(nullptr)
    ,forgotPasswordWindow(nullptr)
    ,mainMenuWindow(nullptr)
    ,/*gameRoomWindow(nullptr)
    ,profileEditorWindow(nullptr)*/
    authManager(new AuthManager(this))
    ,currentWindow(Login)
{
    setupConnections();
    createWindow(Login);
    loginWindow->show();
}
WindowManager::~WindowManager() {

}
void WindowManager::setupConnections() {
    connect(authManager, &AuthManager::loginSuccess, this, &WindowManager::onLoginSuccess);
    connect(authManager, &AuthManager::loginFailed, this, &WindowManager::onLoginFailed);
}
void WindowManager::createWindow(WindowType type) {
    switch(type) {
    case Login:
        if(!loginWindow) {
            loginWindow = new LoginWindow(authManager, nullptr);
            connect(loginWindow, &LoginWindow::signupRequested, this, &WindowManager::switchToSignup);
            connect(loginWindow, &LoginWindow::forgotPasswordRequested, this, &WindowManager::switchToForgotPassword);
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
    // case MainMenu:
    //     if(!mainMenuWindow) {
    //         mainMenuWindow = new MainMenuWindow(authManager, currentPlayer, nullptr);
    //         connect(mainMenuWindow, &MainMenuWindow::logoutRequested, this, &WindowManager::onLogout);
    //         connect(mainMenuWindow, &MainMenuWindow::gameSelected, this, &WindowManager::switchToGameRoom);
    //     }
    //  break;
    // case GameRoom:
    //  break;
    // case ProfileEditor:
    // if(!profileEditorWindow) {
    //    profileEditorWindow = new ProfileEditorWindow(authManager, currentPlayer, nullptr);
    //  connect(profileEditorWindow, &ProfileEditorWindow::closed, this, &WindowManager::onProfileEditorClosed);
    // }
    //  break;
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
    case GameRoom:
        // if(gameRoomWindow) {
        //     gameRoomWindow->show();
        //     gameRoomWindow->raise();
        //     gameRoomWindow->activateWindow();
        // }
        break;
    case ProfileEditor:
        // if(profileEditorWindow) {
        //     profileEditorWindow->show();
        //     profileEditorWindow->raise();
        //     profileEditorWindow->activateWindow();
        // }
        break;
    }
    currentWindow = type;
    emit windowShown(type);

}
void WindowManager::showWindow(WindowType type, const QString& gameType) {
    if(type == GameRoom) {
        // if(gameRoomWindow) {
        //     gameRoomWindow->close();
        //     gameRoomWindow->deleteLater();
        //     gameRoomWindow = nullptr;
        // }
        // gameRoomWindow = new GameRoomWindow(gameType, authManager, currentPlayer, nullptr);
        // connect(gameRoomWindow, &GameRoomWindow::closed, this, &WindowManager::onGameRoomClosed);
        showWindow(GameRoom);
    }
}
void WindowManager::hideAllWindows() {
    if(loginWindow) loginWindow->hide();
    if(signupWindow) signupWindow->hide();
    if(forgotPasswordWindow) forgotPasswordWindow->hide();
    if(mainMenuWindow) mainMenuWindow->hide();
    // if(gameRoomWindow) gameRoomWindow->hide();
    // if(profileEditorWindow) profileEditorWindow->hide();
}
void WindowManager::hideWindow(WindowType type) {
    switch (type) {
    case Login:
        if(loginWindow) loginWindow->hide();
        break;
    case Signup:
        if(signupWindow) signupWindow->hide();
        break;
    case ForgotPassword:
        if(forgotPasswordWindow) forgotPasswordWindow->hide();
        break;
    case MainMenu:
        if(mainMenuWindow) mainMenuWindow->hide();
        break;
    case GameRoom:
        // if(gameRoomWindow) gameRoomWindow->hide();
        break;
    case ProfileEditor:
        // if(profileEditorWindow) profileEditorWindow->hide();
        break;
    }
}
void WindowManager::closeWindow(WindowType type) {
    switch (type) {
    case Login:
        if(loginWindow) {
            loginWindow->close();
            loginWindow->deleteLater();
            loginWindow = nullptr;
        }
        break;
    case Signup:
        if(signupWindow) {
            signupWindow->close();
            signupWindow->deleteLater();
            signupWindow = nullptr;
        }
        break;
    case ForgotPassword:
        if(forgotPasswordWindow) {
            forgotPasswordWindow->close();
            forgotPasswordWindow->deleteLater();
            forgotPasswordWindow = nullptr;
        }
        break;
    case MainMenu:
        if(mainMenuWindow) {
            mainMenuWindow->close();
            mainMenuWindow->deleteLater();
            mainMenuWindow = nullptr;
        }
        break;
    case GameRoom:
        // if(gameRoomWindow) {
        //     gameRoomWindow->close();
        //     gameRoomWindow->deleteLater();
        //     gameRoomWindow = nullptr;
        // }
        break;
    case ProfileEditor:
        // if(profileEditorWindow) {
        //     profileEditorWindow->close();
        //     profileEditorWindow->deleteLater();
        //     profileEditorWindow = nullptr;
        // }
        break;
    }
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
void WindowManager::switchToGameRoom(const QString& gameType, const QString& username) {
    currentPlayer = username;
    showWindow(GameRoom, gameType);
}
void WindowManager::switchToProfileEditor(const QString& username) {
    currentPlayer = username;
    showWindow(ProfileEditor);
}
void WindowManager::onLoginSuccess(const QString& username) {
    currentPlayer = username;
    emit playerLoggedIn(username);
    switchToMainMenu(username);
}
void WindowManager::onLogout() {
    authManager->logout(currentPlayer);
    currentPlayer.clear();
    emit playerLoggedOut();
    switchToLogin();
}
void WindowManager::onSignupCompleted(const QString& username) {
    switchToLogin();
}
void WindowManager::onGameRoomClosed() {
    // if(gameRoomWindow) {
    //     gameRoomWindow->deleteLater();
    //     gameRoomWindow = nullptr;
    // }
    switchToMainMenu(currentPlayer);
}
void WindowManager::onProfileEditorClosed() {
    // if(profileEditorWindow) {
    //     profileEditorWindow->deleteLater();
    //     profileEditorWindow = nullptr;
    // }
    switchToMainMenu(currentPlayer);
}
void WindowManager::setCurrentPlayer(const QString& username) {
    currentPlayer = username;
}