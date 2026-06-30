#include "player.h"

Player::Player():
    isActive(true),
    createdAt(QDateTime::currentDateTime())
{}

Player::Player(const QString& _username, const QString& _password,
       const QString& _name, const QString& _phoneNumber,
       const QString& _email):
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

void Player::setUsername(const QString& _username)
{
    username = _username;
}

void Player::setPassword(const QString& _password)
{
    password = _password;
}

void Player::setName(const QString& _name)
{
    name = _name;
}

void Player::setPhoneNumber(const QString& _phoneNumber)
{
    phoneNumber = _phoneNumber;
}

void Player::setEmail(const QString& _email)
{
    email = _email;
}

void Player::setIsActive(bool _isActive)
{
    isActive = _isActive;
}

void Player::setLastLogin(const QDateTime& _lastLogin)
{
    lastLogin = _lastLogin;
}

QJsonObject Player::toJson() const
{
    QJsonObject obj;
    obj["username"]     = username;
    obj["password"]     = password;
    obj["name"]         = name;
    obj["phoneNumber"]  = phoneNumber;
    obj["email"]        = email;
    obj["isActive"]     = isActive;
    obj["createdAt"]    = createdAt.toString(Qt::ISODate);  //format: YYYY-MM-DDTHH:mm:ss
    obj["lastLogin"]    = lastLogin.toString(Qt::ISODate);  //format: YYYY-MM-DDTHH:mm:ss
    return obj;
}

Player Player::fromJson(const QJsonObject& jsonObj)
{
    Player player;
    player.username     = jsonObj["username"].toString();
    player.password     = jsonObj["password"].toString();
    player.name         = jsonObj["name"].toString();
    player.phoneNumber  = jsonObj["phoneNumber"].toString();
    player.email        = jsonObj["email"].toString();
    player.isActive     = jsonObj["isActive"].toBool(true);

    QString createdAtStr = jsonObj["createdAt"].toString();
    if (!createdAtStr.isEmpty())
    {
        player.createdAt = QDateTime::fromString(createdAtStr, Qt::ISODate);
    }

    QString lastLoginStr = jsonObj["lastLogin"].toString();
    if (!lastLoginStr.isEmpty())
    {
        player.lastLogin = QDateTime::fromString(lastLoginStr, Qt::ISODate);
    }

    return player;
}

bool Player::operator==(const Player& other) const
{
    return this->username == other.username;
}

bool Player::operator!=(const Player& other) const
{
    return this->username != other.username;
}