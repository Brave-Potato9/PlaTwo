#ifndef PASSWORDHASHER_H
#define PASSWORDHASHER_H
#include <QString>
#include <QCryptographicHash>
class PasswordHasher
{
public:
    static QString hashPassword(const QString& password);
    static bool verifyPassword(const QString& password, const QString& hash);
    static QString generateSalt();
};

#endif // PASSWORDHASHER_H
