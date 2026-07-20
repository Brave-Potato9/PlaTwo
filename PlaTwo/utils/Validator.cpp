#include "Validator.h"
#include <QRegularExpression>
#include <QHostAddress>
bool Validator::isValidPhone(const QString& phone) {
    QRegularExpression regex("^09[0-9]{9}$");
    return regex.match(phone).hasMatch();
}
bool Validator::isValidEmail(const QString& email) {
    QRegularExpression regex("^[a-zA-z0-9._%+-]+@[a-zA-z0-9.-]+\\.[a-zA-Z]{2,}$");
    return regex.match(email).hasMatch();
}
bool Validator::isValidUsername(const QString& username) {
    QRegularExpression regex("^[a-zA-Z0-9_]{3,20}$");
    return regex.match(username).hasMatch();
}
bool Validator::isValidPassword(const QString& password) {
    QRegularExpression regex(
        "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$"
        );
    return regex.match(password).hasMatch();
}
bool Validator::isValidIP(const QString& ip) {
    QHostAddress address(ip);
    return !address.isNull();
}
bool Validator::isValidPort(int port) {
    return port >= 1024 && port <= 65535;
}
