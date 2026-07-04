#include "Room.h"

//------------------------------------ constructor ------------------------------------
Room::Room(const QString& roomId, const GameConfig& config, QObject * parent) : QObject(parent), roomId(roomId), gameConfig(config), gameState(Game::State::Idle) {}

//------------------------------------ getters ------------------------------------
QString Room::getRoomId() const {
    return roomId;
}
QString Room::getHost() const {
    return host;
}
QStringList Room::getPlayers() const {
    return players;
}
GameConfig Room::getGameConfig() const {
    return gameConfig;
}
bool Room::isFull() const {
    return players.size() == 2;
}
bool Room::isEmpty() const {
    return players.isEmpty();
}
Game::State Room::getGameState() const {
    return gameState;
}

//------------------------------------ working_with_timer_and_activity ------------------------------------
bool Room::addPlayer(const QString& username) {
    if(isFull()) return false;
    if(isPlayerInRoom(username)) return false;
    players.append(username);
    if(players.size() == 1) {
        host = username;
    }
    emit playerJoined(username);
    return true;
}
bool Room::removePlayer(const QString& username) {
    if(!isPlayerInRoom(username)) return false;
    players.removeAll(username);
    if(host == username && !players.isEmpty()) {
        host = players[0];
    }
    emit playerLeft(username);
    return true;
}
bool Room::isPlayerInRoom(const QString& username) const {
    return players.contains(username);
}


//------------------------------------ setters ------------------------------------
void Room::setHost(const QString& username) {
    if(isPlayerInRoom(username)) {
        host = username;
    }
}
void Room::setGameState(Game::State state) {
    gameState = state;
    if(state == Game::State::Playing) {
        emit gameStarted();
    } else if(state == Game::State::GameOver) {

    }

}
