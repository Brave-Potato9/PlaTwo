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
