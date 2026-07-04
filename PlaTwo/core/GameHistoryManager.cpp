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

//------------------------------------ specifice_history_methods ------------------------------------
GameHistory GameHistoryManager::getLastHistory(const QString& username, const QString& gameType) const
{
    QList<GameHistory> histories = loadHistories(username, gameType);
    if (histories.isEmpty())
    {
        return GameHistory();
    }

    return histories.last();
}

QList<GameHistory> GameHistoryManager::getHistoriesByWinner(const QString& username, const QString& gameType, const QString& winner) const
{
    const QList<GameHistory> histories = loadHistories(username, gameType);
    QList<GameHistory> result;

    for (const GameHistory& history : histories)
    {
        if (history.getWinner() == winner)
        {
            result.append(history);
        }
    }

    return result;
}

//------------------------------------ statistics ------------------------------------
int GameHistoryManager::getTotalGamesCount(const QString& username, const QString& gameType) const
{
    return loadHistories(username, gameType).size();
}

int GameHistoryManager::getWinCount(const QString& username, const QString& gameType) const
{
    const QList<GameHistory> histories = loadHistories(username, gameType);
    int count = 0;

    for (const GameHistory& history : histories)
    {
        if (history.getWinner() == username)
        {
            count++;
        }
    }

    return count;
}

int GameHistoryManager::getLossCount(const QString& username, const QString& gameType) const
{
    const QList<GameHistory> histories = loadHistories(username, gameType);
    int count = 0;

    for (const GameHistory& history : histories)
    {
        QString winner = history.getWinner();
        if (!winner.isEmpty() && winner != username && winner != "Draw")
        {
            count++;
        }
    }

    return count;
}

int GameHistoryManager::getDrawCount(const QString& username, const QString& gameType) const
{
    const QList<GameHistory> histories = loadHistories(username, gameType);
    int count = 0;

    for (const GameHistory& history : histories)
    {
        if (history.getWinner() == "Draw")
        {
            count++;
        }
    }

    return count;
}

int GameHistoryManager::getTotalScore(const QString& username, const QString& gameType) const
{
    const QList<GameHistory> histories = loadHistories(username, gameType);
    int total = 0;

    for (const GameHistory& history : histories)
    {
        QStringList players = history.getPlayers();
        int playerIndex = players.indexOf(username);
        if (playerIndex != -1 && playerIndex < history.getScores().size())
        {
            total += history.getScores()[playerIndex];
        }
    }

    return total;
}

double GameHistoryManager::getWinRate(const QString& username, const QString& gameType) const
{
    int total = getTotalGamesCount(username, gameType);
    if (total == 0) {
        return 0.0;
    }

    int wins = getWinCount(username, gameType);
    return (static_cast<double>(wins) / total) * 100.0;
}