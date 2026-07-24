#include "PlayerManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
//------------------------------------ constructor ------------------------------------
PlayerManager::PlayerManager(QObject* parent) : QObject(parent), filePath("players.json") {
    loadPlayers();
}

//------------------------------------ load_and_save_players ------------------------------------
bool PlayerManager::loadPlayers() {
    QFile file(filePath);
    if(!file.exists()) {
        return true;
    }
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open players file for reading";
        return false;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isArray()) {
        return false;
    }
    players.clear();
    const QJsonArray array = doc.array();
    for(const auto& a : array) {
        Player player = Player::fromJson(a.toObject());
        players.append(player);
    }
    return true;
}
bool PlayerManager::savePlayers() {
    QJsonArray array;
    for(const auto& player : players) {
        array.append(player.toJson());
    }
    QJsonDocument doc(array);
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open players file for writing";
        return false;
    }
    file.write(doc.toJson());
    file.close();
    return true;
}

//------------------------------------ add_remove_and_player ------------------------------------
bool PlayerManager::addPlayer(const Player& player) {
    if(playerExists(player.getUsername())) {
        return false;
    }
    players.append(player);
    bool success = savePlayers();
    if(success) {
        emit playerAdded(player.getUsername());
    }
    return success;
}
bool PlayerManager::removePlayer(const QString& username) {
    for (int i=0 ; i < players.size() ; i++ ) {
        if(players[i].getUsername() == username) {
            players.removeAt(i);
            bool success = savePlayers();
            if(success) {
                emit playerRemoved(username);
            }
            return success;
        }
    }
    return false;
}
bool PlayerManager::updatePlayer(const Player& player) {
    for (int i=0 ; i < players.size() ; i++ ) {
        if(players[i].getUsername() == player.getUsername()) {
            players[i] = player;
            bool success = savePlayers();
            if(success) {
                emit playerUpdated(player.getUsername());
            }
            return success;
        }
    }
    return false;
}

//------------------------------------ check_Exists_and_find ------------------------------------
bool PlayerManager::findPlayer(const QString& username, Player& foundedPlayer) const {
    for(const auto& player : players) {
        if(player.getUsername() == username && player.getIsActive()) {
            foundedPlayer = player;
            return true;
        }
    }
    return false;
}
bool PlayerManager::playerExists(const QString& username) {
    for(const auto& player : players) {
        if(player.getUsername() == username) {
            return true;
        }
    }
    return false;
}

//------------------------------------ get_all_and_active_players ------------------------------------
QList<Player>& PlayerManager::getAllPlayers() {
    return players;
}
QList<Player> PlayerManager::getActivePlayers() const {
    QList<Player> activePlayers;
    for(const auto& player : players) {
        if(player.getIsActive()) {
            activePlayers.append(player);
        }
    }
    return activePlayers;
}

//------------------------------------ update_last_login ------------------------------------
void PlayerManager::updateLastLogin(const QString& username) {
    for(auto& player : players) {
        if(player.getUsername() == username) {
            player.setLastLogin(QDateTime::currentDateTime());
            savePlayers();
            break;
        }
    }
}

