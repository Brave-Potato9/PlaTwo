#ifndef GAMEHISTORYMANAGER_H
#define GAMEHISTORYMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include "GameHistory.h"

class GameHistoryManager: public QObject
{
    Q_OBJECT
private:
    //helper_methods
    QString ensureDirectoryExists(const QString& username) const;
    QList<GameHistory> readAllHistories(const QString& filePath) const;
    bool writeAllHistories(const QString& filePath, const QList<GameHistory>& histories) const;

public:
    //constructors
    explicit GameHistoryManager(QObject* parent = nullptr);

    //save_and_load_methods
    bool saveHistory(const QString& username, const QString& gameType, const GameHistory& history);
    QList<GameHistory> loadHistories(const QString& username, const QString& gameType) const;

    //specifice_history_methods
    GameHistory getLastHistory(const QString& username, const QString& gameType) const;
    QList<GameHistory> getHistoriesByWinner(const QString& username, const QString& gameType, const QString& winner) const;

    //statistics
    int getTotalGamesCount(const QString& username, const QString& gameType) const;
    int getWinCount(const QString& username, const QString& gameType) const;
    int getLossCount(const QString& username, const QString& gameType) const;
    int getDrawCount(const QString& username, const QString& gameType) const;
    int getTotalScore(const QString& username, const QString& gameType) const;
    double getWinRate(const QString& username, const QString& gameType) const;

    //file_management_methods
    QString getHistoryFilePath(const QString& username, const QString& gameType) const;
    bool deleteHistory(const QString& username, const QString& gameType, int index);

signals:
    void historySaved(const QString& username, const QString& gameType);
    void historyDeleted(const QString& username, const QString& gameType, int index);
};

#endif // GAMEHISTORYMANAGER_H
