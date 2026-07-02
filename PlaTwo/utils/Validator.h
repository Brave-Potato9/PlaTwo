#ifndef VALIDATOR_H
#define VALIDATOR_H
#include <QString>
class Validator
{
public:
    static bool isValidPhone(const QString& phone);
    static bool isValidEmail(const QString& email);
    static bool isValidUsername(const QString& username);
    static bool isValidPassword(const QString& password);
    static bool isValidIP(const QString& ip);
    static bool isValidPort(int port);

};

#endif // VALIDATOR_H
