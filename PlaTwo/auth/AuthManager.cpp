#include "AuthManager.h"
#include <QDebug>
#include <QRegularExpression>
#include "PasswordHasher.h"
//------------------------------------ constructor ------------------------------------
AuthManager::AuthManager(QObject* parent): QObject(parent) {}

//------------------------------------ signup_login_logout ------------------------------------
bool AuthManager::signup(const QString& username, const QString& password, const QString& name, const QString& phone, const QString& email) {
    if(username.isEmpty() || password.isEmpty() || name.isEmpty()) {
        emit signupFailed("All fields should be fill");
        return false;
    }
    if(!validatePhone(phone)) {
        emit signupFailed("Phone number is not valid");
        return false;
    }
    if(!validateEmail(email)) {
        emit signupFailed("Email address is not valid");
        return false;
    }
    if(!validatePassword(password)) {
        emit signupFailed("Password must have at least 8 characters(at least one of small and captial letters, digits and special characters");
        return false;
    }
    if(playerManager.playerExists(username)) {
        emit signupFailed(" This username had saved. Please choose another !");
        return false;
    }
    QString passwordHash = PasswordHasher::hashPassword(password);
    Player newPlayer(username, passwordHash, name, phone, email);
    if(playerManager.addPlayer(newPlayer)) {
        emit signupSuccess(username);
        return true;
    }
    emit signupFailed("Error in signup");
    return false;
}
bool AuthManager::login(const QString& username, const QString& password) {
    if(username.isEmpty() || password.isEmpty()) {
        emit loginFailed("Username and password are required");
        return false;
    }
    Player player;
    if(!playerManager.findPlayer(username, player)) {
        emit loginFailed("Player not found");
        return false;
    }
    if(!PasswordHasher::verifyPassword(password, player.getPassword() )) {
        emit loginFailed("Password is not correct");
        return false;
    }
    currentPlayer = username;
    playerManager.updateLastLogin(username);
    emit loginSuccess(username);
    return true;
}
bool AuthManager::loginWithHash(const QString& username, const QString& passwordHash)
{
    Player player;
    if (!playerManager.findPlayer(username, player)) {
        return false;
    }

    if (player.getPassword() != passwordHash) {
        return false;
    }

    currentPlayer = username;
    playerManager.updateLastLogin(username);
    emit loginSuccess(username);
    return true;
}
bool AuthManager::isLoggedIn(const QString& username) const {
    return username == currentPlayer;
}
bool AuthManager::logout(const QString& username) {
    if(username == currentPlayer) {
        currentPlayer = "";
        emit logoutSuccess(username);
        return true;
    }
    return false;
}

//------------------------------------ get_current_player_and_data ------------------------------------
Player AuthManager::getCurrentPlayerData() const {
    Player player;
    playerManager.findPlayer(currentPlayer, player);
    return player;
}

//------------------------------------ change_and_reset_password ------------------------------------
bool AuthManager::changePassword(const QString& username, const QString& oldPassword, const QString& newPassword) {
    Player player;
    if(!playerManager.findPlayer(username, player)) {
        return false;
    }
    if(!PasswordHasher::verifyPassword(oldPassword, player.getPassword())) {
        return false;
    }
    if(!validatePassword(newPassword)) {
        return false;
    }
    player.setPassword(PasswordHasher::hashPassword((newPassword)));
    if(playerManager.updatePlayer(player)) {
        emit passwordChanged(username);
        return true;
    }
    return false;
}
bool AuthManager::resetPassword(const QString& phone, const QString& newPassword) {
    for(auto& player: playerManager.getAllPlayers()) {
        if(player.getPhoneNumber() == phone && player.getIsActive()) {
            if(!validatePassword(newPassword)) {
                return false;
            }
            player.setPassword(PasswordHasher::hashPassword(newPassword));
            if(playerManager.updatePlayer(player)) {
                emit passwordChanged(player.getUsername());
                return true;
            }
        }
    }
    return false;
}
//------------------------------------ check_phone_email_password ------------------------------------
bool AuthManager::validatePhone(const QString& phone) const {
    QRegularExpression regex("^09[0-9]{9}$");
    return regex.match(phone).hasMatch();
}
bool AuthManager::validateEmail(const QString& email) const {
    QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return regex.match(email).hasMatch();
}
bool AuthManager::validatePassword(const QString& password) const {
    QRegularExpression regex(
        "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$"
        );
    return regex.match(password).hasMatch();
}
