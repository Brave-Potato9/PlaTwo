#include "GameHistory.h"

//------------------------------------ constructor ------------------------------------
GameHistory::GameHistory():
    scores(2, 0)
{}

GameHistory::GameHistory(const QStringList& players, const QString& gameType)
    : players(players)
    , gameType(gameType)
    , scores(2, 0)
{}

//------------------------------------ getter ------------------------------------
QStringList GameHistory::getPlayers() const
{
    return players;
}

QDateTime GameHistory::getStartTime() const
{
    return startTime;
}

QDateTime GameHistory::getEndTime() const
{
    return endTime;
}

QString GameHistory::getWinner() const
{
    return winner;
}

QString GameHistory::getGameType() const
{
    return gameType;
}

QVector<int> GameHistory::getScores() const
{
    return  scores;
}

QList<Move> GameHistory::getMoves() const
{
    return moves;
}