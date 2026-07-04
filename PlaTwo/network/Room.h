#ifndef ROOM_H
#define ROOM_H
#include <QObject>
#include <QString>
#include <QStringList>
#include "../core/Game.h"
#include "../utils/GameConfig.h"
class Room : public QObject
{
    Q_OBJECT
private:
    QString roomId;
    QString host;
    QStringList players;
    GameConfig gameConfig;
    Game::State gameState;
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

    //add_remove_check_player
    bool addPlayer(const QString& username);
    bool removePlayer(const QString& username);
    bool isPlayerInRoom(const QString& username) const;

    //setters
    void setHost(const QString& username);
    void setGameState(Game::State state);

signals:
    void playerJoined(const QString& username);
    void playerLeft(const QString& username);
    void gameStarted();
    void gameEnded(const QString& winner);


};

#endif // ROOM_H
