#include "GameHistoryManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

//------------------------------------ constructor ------------------------------------
GameHistoryManager::GameHistoryManager(QObject* parent)
    : QObject(parent)
{}

//------------------------------------ helper_methods ------------------------------------
QString GameHistoryManager::ensureDirectoryExists(const QString& username) const
{
    QString dirPath = "data/" + username;
    QDir dir(dirPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    return dirPath;
}

QList<GameHistory> GameHistoryManager::readAllHistories(const QString& filePath) const
{
    QList<GameHistory> histories;

    QFile file(filePath);
    if (!file.exists())
    {
        return histories;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        return histories;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray())
    {
        return histories;
    }

    const QJsonArray array = doc.array();
    for (const auto& item : array)
    {
        if (item.isObject())
        {
            GameHistory history = GameHistory::fromJson(item.toObject());
            histories.append(history);
        }
    }

    return histories;
}

bool GameHistoryManager::writeAllHistories(const QString& filePath, const QList<GameHistory>& histories) const
{
    QJsonArray array;
    for (const GameHistory& history : histories)
    {
        array.append(history.toJson());
    }

    QJsonDocument doc(array);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.write(doc.toJson());
    file.close();
    return true;
}

//------------------------------------ save_and_load_methods ------------------------------------
bool GameHistoryManager::saveHistory(const QString& username, const QString& gameType, const GameHistory& history)
{
    QString filePath = getHistoryFilePath(username, gameType);
    ensureDirectoryExists(username);

    QList<GameHistory> histories = readAllHistories(filePath);
    histories.append(history);

    bool success = writeAllHistories(filePath, histories);
    if (success)
    {
        emit historySaved(username, gameType);
    }
    return success;
}

QList<GameHistory> GameHistoryManager::loadHistories(const QString& username, const QString& gameType) const
{
    QString filePath = getHistoryFilePath(username, gameType);
    return readAllHistories(filePath);
}