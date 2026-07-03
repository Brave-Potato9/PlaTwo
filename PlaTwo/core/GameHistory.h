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
    void setPlayers(const QStringList& value) ;
    void setStartTime(const QDateTime& value) ;
    void setEndTime(const QDateTime& value) ;
    void setWinner(const QString& value) ;
    void setGameType(const QString& value) ;
    void setScores(const QVector<int>& value) ;
    void setMoves(const QList<Move>& value) ;

    //misc
    void addMove(const Move& move) ;

    //working_with_JSON
    QJsonObject toJson() const;
    static GameHistory fromJson(const QJsonObject& obj);
};

#endif // GAMEHISTORY_H
