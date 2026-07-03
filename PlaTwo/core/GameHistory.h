#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include "Move.h"

class GameHistory
{
private:
    QStringList players;
    QDateTime startTime;
    QDateTime endTime;
    QString winner;
    QString gameType;
    QVector<int> scores;
    QList<Move> moves;

public:
    //constructor
    GameHistory();
    GameHistory(const QStringList& players, const QString& gameType);

    //getter
    QStringList getPlayers() const ;
    QDateTime getStartTime() const ;
    QDateTime getEndTime() const ;
    QString getWinner() const ;
    QString getGameType() const ;
    QVector<int> getScores() const ;
    QList<Move> getMoves() const ;

    //setter
    void setPlayers(const QStringList& _players) ;
    void setStartTime(const QDateTime& _startTime) ;
    void setEndTime(const QDateTime& _endTime) ;
    void setWinner(const QString& _winner) ;
    void setGameType(const QString& _gameType) ;
    void setScores(const QVector<int>& _scores) ;
    void setMoves(const QList<Move>& _moves) ;

    //misc
    void addMove(const Move& move) ;

    //working_with_JSON
    QJsonObject toJson() const;
    static GameHistory fromJson(const QJsonObject& obj);
};

#endif // GAMEHISTORY_H
