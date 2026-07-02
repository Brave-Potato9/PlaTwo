#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QDateTime>
#include "Move.h"
#include "/utils/GameConfig.h"

class Game
{
    Q_OBJECT
public:
    enum class State
    {
        Idle,
        Playing,            //while playing
        Paused,
        GameOver,
        Aborted             //game cancled
    };

protected:
    //game_management_attribut
    QString gameType;
    State state;

    //history_attribute
    QList<Move> moveHistory;
    bool isGameOver;
    QString winner;

    QVector<int> scores;

    //time_attribute
    QDateTime startTime;
    QDateTime endTime;

    //helper_method
    virtual void onGameStart();
    virtual void onGameEnd(const QString& winner);
    void clearScores();

public:
    //constructor
    explicit Game(QObject* parent = nullptr);

    //destructor
    virtual ~Game();

    //main_method
    virtual void initialize(const GameConfig& config) = 0;
    virtual bool makeMove(const Move& move) = 0;
    virtual bool isValidMove(const Move& move) const = 0;
    virtual QList<Move> getPossibleMoves() const = 0;
    virtual bool checkGameOver() = 0;
    virtual QString getWinner() const = 0;

    //supplementry_methods
    virtual QJsonObject getBoardState() const = 0;
    virtual void resetGame() = 0;
    virtual QString getGameType() const = 0;

    //score_management_methods
    QVector<int> getScores() const;
    int getScore(int playerIndex) const;
    void addScore(int playerIndex, int points);
    void setScore(int playerIndex, int points);

    //state_management_methods
    State getState() const ;
    void setState(State newState);
    bool isFinished() const;
    bool isPlaying() const ;
    virtual QString getStateString() const;

    //history_management_methods
    QList<Move> getMoveHistory() const ;
    void addMoveToHistory(const Move& move);
    int getMoveCount() const ;
    void clearMoveHistory();

    //time_management_methods
    QDateTime getStartTime() const ;
    QDateTime getEndTime() const ;
    void setStartTime(const QDateTime& time) ;
    void setEndTime(const QDateTime& time) ;
    qint64 getDuration() const;

signals:
    //move_signals
    void moveExecuted(const Move& move);
    void invalidMoveAttempted(const Move& move, const QString& reason);
    void possibleMovesUpdated(const QList<Move>& moves);

    //main_game_signals
    void gameStarted();
    void gameLogicOver(const QString& winner);
    void gameReset();
    void stateChanged(State newState);

    //score_signals
    void scoreUpdated(int playerIndex, int newScore);
    void scoresUpdated(const QVector<int>& scores);

    //board_signals
    void boardUpdated();
    void boardStateChanged(const QJsonObject& boardState);

};

#endif // GAME_H
