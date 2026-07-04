#ifndef GAMESESSION_H
#define GAMESESSION_H

#include <QObject>
#include <QString>
#include <QList>
#include <QDateTime>
#include "../core/Game.h"
#include "../core/Move.h"
#include "../utils/TimerManager.h"
#include "../utils/GameConfig.h"
#include "../core/GameHistory.h"
#include "../core/GameHistoryManager.h"

class Room;

class GameSession: public QObject
{
    Q_OBJECT
public:
    enum class SessionState {
        Idle,
        WaitingForPlayers,
        Starting,
        Playing,
        Paused,
        GameOver,
        Aborted
    };

private:
    //session_management
    SessionState sessionState;
    GameConfig config;

    //have
    Game* game;
    Room* room;
    TimerManager timerManager;

    //player_management
    QList<QString> players;
    int currentPlayerIndex;

    //time_management
    bool hasTimeLimit;
    int timeLimitPerPlayer;

    //history_management
    GameHistory history;
    GameHistoryManager historyManager;

    //helper_methods
    void initializeGame(const GameConfig& config);
    void cleanUp();
    void saveHistory(const QString& winnerOverride = QString());
    void broadcastGameState(const QString& state);
    void broadcastMove(const Move& move);
    void broadcastGameEnded(const QString& winner);
    bool checkGameEnd();
    QString getStateString(SessionState state) const;

public:
    //constructors
    explicit GameSession(Game* game, Room* room, QObject* parent = nullptr);

    //destructors
    ~GameSession();

    //session_management_methods
    bool startSession(const GameConfig& config);
    void endSession(const QString& winner = QString());
    void pauseSession();
    void resumeSession();
    void abortSession();

    //player_management_methods
    void addPlayer(const QString& username);
    void removePlayer(const QString& username);
    QList<QString> getPlayers() const ;
    bool isPlayerInSession(const QString& username) const;
    int getPlayerCount() const;

    //turn_order_management_methods
    bool makeMove(const Move& move);
    void switchTurn();
    int getCurrentPlayerIndex() const ;
    QString getCurrentPlayer() const;
    bool isPlayerTurn(const QString& username) const;
    void setCurrentPlayer(int index);

    //time_management_methods
    void setTimeLimit(int seconds);
    int getRemainingTime(int playerIndex) const;
    void startTimerForPlayer(int playerIndex);
    void stopTimer();

    //state_management_methods
    SessionState getSessionState() const ;
    bool isActive() const ;
    bool isFinished() const ;
    QString getStateString() const;

    //getters
    Game* getGame() const ;
    Room* getRoom() const ;
    GameConfig getConfig() const ;
    GameHistory getHistory() const ;
    QList<Move> getMoveHistory() const ;

signals:
    //session_management_signals
    void sessionStarted();
    void sessionEnded(const QString& winner);
    void sessionPaused();
    void sessionResumed();
    void sessionAborted();
    void sessionStateChanged(GameSession::SessionState newState);

    //turn_and_move_management_signals
    void turnChanged(int playerIndex, const QString& username);
    void moveProcessed(const Move& move, bool success);
    void moveFailed(const Move& move, const QString& reason);
    void moveReceived(const Move& move);

    //time_management_signals
    void timeUpdated(int playerIndex, int remainingSeconds);
    void timeExpired(int playerIndex, const QString& username);

    //player_management_signals
    void playerJoined(const QString& username);
    void playerLeft(const QString& username);

    //state_management_signals
    void gameStateChanged(const QString& state);
    void errorOccurred(const QString& error);

private slots:
    void onGameOver(const QString& winner);
    void onTimerExpired(int playerIndex);
    void onTimerUpdated(int playerIndex, int seconds);
    void onMoveExecuted(const Move& move);
};

#endif // GAMESESSION_H
