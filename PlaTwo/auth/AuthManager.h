#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H
#include "PlayerManager.h"
#include "PasswordHasher.h"
#include <QObject>
class AuthManager : public QObject
{
    Q_OBJECT
private:
    PlayerManager playerManager;
    QString currentPlayer;

    //check_phone_email_password
    bool validatePhone(const QString& phone) const;
    bool validateEmail(const QString& email) const;
    bool validatePassword(const QString& password) const;

public:
    //constructor
    explicit AuthManager(QObject * parent = nullptr);

    //signup_login_logout
    bool signup(const QString& username, const QString& password, const QString& name, const QString& phone, const QString& email);
    bool login(const QString& username, const QString& password);
    bool loginWithHash(const QString& username, const QString& passwordHash);
    bool isLoggedIn(const QString& username) const ;
    bool logout(const QString& username);

    //change_and_reset_password
    bool changePassword(const QString& username, const QString& oldPassword, const QString& newPassword);
    bool resetPassword(const QString& phone, const QString& newPassword);

    //get_current_player_and_data
    QString getCurrentPlayer() const {return currentPlayer;}
    Player getCurrentPlayerData() const;
    PlayerManager& getPlayerManager() {return playerManager;}

signals:
    void loginSuccess(const QString& username);
    void loginFailed(const QString& reason);
    void signupSuccess(const QString& username);
    void signupFailed(const QString& reason);
    void logoutSuccess(const QString& username);
    void passwordChanged(const QString& username);


};

#endif // AUTHMANAGER_H
