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

//------------------------------------ getter ------------------------------------
void GameHistory::setPlayers(const QStringList& _players)
{
    players = _players;
}

void GameHistory::setStartTime(const QDateTime& _startTime)
{
    startTime = _startTime;
}

void GameHistory::setEndTime(const QDateTime& _endTime)
{
    endTime - _endTime;
}

void GameHistory::setWinner(const QString& _winner)
{
    winner = _winner;
}

void GameHistory::setGameType(const QString& _gameType)
{
    gameType = _gameType;
}

void GameHistory::setScores(const QVector<int>& _scores)
{
    scores = _scores;
}
\
void GameHistory::setMoves(const QList<Move>& _moves)
{
    moves = _moves;
}