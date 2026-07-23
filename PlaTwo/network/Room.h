#ifndef ROOM_H
#define ROOM_H
#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include "../core/Game.h"
#include "../utils/GameConfig.h"
#include "../core/GameSession.h"
class Room : public QObject
{
    Q_OBJECT
private:
    QString roomId;
    QString host;
    QStringList players;
    GameConfig gameConfig;
    Game::State gameState;
    GameSession* gameSession;
public:
    //constructor
    explicit Room(const QString& roomId, const GameConfig& config, QObject * parent = nullptr);

    //getters
    QString getRoomId() const;
    QString getHost() const;
    QStringList getPlayers() const;
    GameConfig getGameConfig() const;
    bool isFull() const;
    bool isEmpty() const;
    Game::State getGameState() const;
    GameSession* getGameSession() const;

    //add_remove_check_player
    bool addPlayer(const QString& username);
    bool removePlayer(const QString& username);
    bool isPlayerInRoom(const QString& username) const;

    //setters
    void setHost(const QString& username);
    void setGameState(Game::State state);

    //game_management
    bool startGame(const GameConfig& config);
    void processMove(const Move& move);
    void notifyGameEnded(const QString& winner);
    // senders
    void sendColorUpdate(const QString& username, const QString& color);
    void sendReadyStatus(const QString& username, bool ready);
    void sendBoardState(const QJsonObject& boardState);
    void sendGamePaused();
    void sendGameResumed();
    void sendSyncRequest();
    void sendMessageToAll(const QJsonObject& message);

signals:
    void playerJoined(const QString& username);
    void playerLeft(const QString& username);
    void gameStarted();
    void gameEnded(const QString& winner);
    void messageToAll(const QByteArray& message);
    void colorUpdated(const QString& username, const QString& color);
    void playerReadyChanged(const QString& username, bool ready);
    void boardStateUpdated(const QJsonObject& boardState);
    void gamePaused();
    void gameResumed();
    void syncRequested();

};

#endif // ROOM_H
