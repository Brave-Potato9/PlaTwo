#include "player.h"

Player::Player():
    isActive(true),
    createdAt(QDateTime::currentDateTime())
{}

Player::Player(const QString& _username, const QString& _password,
       const QString& _name, const QString& _phoneNumber, const QString& _email):
    username(_username),
    password(_password),
    name(_name),
    phoneNumber(_phoneNumber),
    email(_email),
    isActive(true),
    createdAt(QDateTime::currentDateTime())
{}

QString Player::getUsername() const
{
    return username;
}

QString Player::getPassword() const
{
    return password;
}

QString Player::getName() const
{
    return name;
}

QString Player::getPhoneNumber() const
{
    return phoneNumber;
}

QString Player::getEmail() const
{
    return email;
}

bool Player::getIsActive() const
{
    return isActive;
}

QDateTime Player::getCreatedAt() const
{
    return createdAt;
}

QDateTime Player::getLastLogin() const
{
    return lastLogin;
}
