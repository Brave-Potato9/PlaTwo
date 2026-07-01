#include "PasswordHasher.h"
#include <QRandomGenerator>
#include <QStringList>
QString PasswordHasher::hashPassword(const QString& password) {
    QString salt = generateSalt();
    QString saltedPassword = password + salt;
    QByteArray hash = QCryptographicHash::hash(saltedPassword.toUtf8(), QCryptographicHash::Sha256);
    return salt + ":" + hash.toHex();
}
bool PasswordHasher::verifyPassword(const QString& password, const QString& hash) {
    QStringList parts = hash.split(":");
    if(parts.size() != 2 ) return false;
    QString salt = parts[0];
    QString savedHash = parts[1];
    QString saltedPassword = password + salt;
    QByteArray createdHash = QCryptographicHash::hash(saltedPassword.toUtf8(), QCryptographicHash::Sha256);
    return savedHash == createdHash.toHex();
}
QString PasswordHasher::generateSalt() {
    const QString characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString salt;
    for (int i=0 ; i < 16 ; i++) {
        int index = QRandomGenerator::global()->bounded(characters.length());
        salt.append(characters[index]);
    }
    return salt;
}
