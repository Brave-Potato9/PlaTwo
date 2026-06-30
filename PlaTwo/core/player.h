#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QJsonObject>
#include <QDateTime>

class Player
{
private:
    QString username;
    QString password;
    QString name;
    QString phoneNumber;
    QString email;
    bool isActive;
    QDateTime createdAt;
    QDateTime lastLogin;

public:
    //constructors
    Player();
    Player(const QString& _username, const QString& _password,
         const QString& _name, const QString& _phoneNumber, const QString& _email);

    //getters
    QString getUsername() const ;
    QString getPassword() const ;
    QString getName() const ;
    QString getPhoneNumber() const ;
    QString getEmail() const ;
    bool getIsActive() const ;
    QDateTime getCreatedAt() const ;
    QDateTime getLastLogin() const ;

    //setter
    void setUsername(const QString& _username) ;
    void setPassword(const QString& _password) ;
    void setName(const QString& _name) ;
    void setPhoneNumber(const QString& _phoneNumber) ;
    void setEmail(const QString& _email) ;
    void setIsActive(bool _isActive) ;
    void setLastLogin(const QDateTime& _lastLogin) ;

    //working_with_JSON
    QJsonObject toJson() const;
    static Player fromJson(const QJsonObject& JsonFile);

    //operators
    bool operator==(const Player& other) const;
    bool operator!=(const Player& other) const;

};

#endif // PLAYER_H
