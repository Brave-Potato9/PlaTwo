#include "NetworkManager.h"
//------------------------------------ constructor ------------------------------------

NetworkManager::NetworkManager(QObject * parent) :QObject(parent)
    ,isServerMode(false)
    ,client(new Client(this))
    ,server(new Server(this)){

    //connect_server_signals
    connect(server, &Server::clientConnected, this, &NetworkManager::clientConnected);
    connect(server, &Server::clientDisconnected, this, &NetworkManager::clientDisconnected);
    connect(server, &Server::roomCreated, this, &NetworkManager::roomCreated);
    connect(server, &Server::roomDestroyed, this, &NetworkManager::roomDestroyed);

    //connect_client_signals
    connect(client, &Client::connected, this, &NetworkManager::connectedToServer);
    connect(client, &Client::disconnected, this, &NetworkManager::disconnectedFromServer);
    connect(client, &Client::connectionFailed, this, &NetworkManager::connectionFailed);
    connect(client, &Client::joinedRoom, this, &NetworkManager::joinedRoom);
    connect(client, &Client::joinFailed, this, &NetworkManager::joinFailed);
    connect(client, &Client::moveReceived, this, &NetworkManager::moveReceived);
    connect(client, &Client::playerJoined, this, &NetworkManager::playerJoinedRoom);
    connect(client, &Client::playerLeft, this, &NetworkManager::playerLeftRoom);
    connect(client, &Client::gameStarted, this, &NetworkManager::gameStarted);
    connect(client, &Client::gameEnded, this, &NetworkManager::gameEnded);

    connect(client, &Client::colorUpdated, this, &NetworkManager::colorUpdated);
    connect(client, &Client::playerReadyChanged, this, &NetworkManager::playerReadyChanged);
    connect(client, &Client::boardStateReceived, this, &NetworkManager::boardStateReceived);
    connect(client, &Client::gamePaused, this, &NetworkManager::gamePaused);
    connect(client, &Client::gameResumed, this, &NetworkManager::gameResumed);
    connect(client, &Client::syncRequested, this, &NetworkManager::syncRequested);

}

//------------------------------------ manage_server ------------------------------------
bool NetworkManager::startServer(int port) {
    isServerMode = true;
    return server->startServer(port);
}
void NetworkManager::stopServer(){
    server->stopServer();
}
bool NetworkManager::isServerRunning() const {
    if (server) {
        return server->isRunning();
    }
    return false;
}
QString NetworkManager::getServerIP() const {
    return server->getIP();
}

//------------------------------------ manage_client ------------------------------------
bool NetworkManager::connectToServer(const QString& ip, int port) {
    isServerMode = false;
    return client->connectToServer(ip, port);
}
void NetworkManager::disconnectFromServer() {
    client->disconnectFromServer();
}
bool NetworkManager::isClientConnected() const {
    return client->isConnected();
}

//------------------------------------ manage_rooms ------------------------------------

bool NetworkManager::createRoom(const QString& roomId, const GameConfig& config, const QString& host) {
    if(isServerMode) {
        return server->createRoom(roomId, config, host);
    }
    return false;
}
bool NetworkManager::joinRoom(const QString roomId, const QString& username) {
    if(isServerMode) {
        return server->joinRoom(roomId, username);
    } else {
        return client->joinRoom(roomId, username);
    }
}
bool NetworkManager::leaveRoom() {
    if(isServerMode) {
        return false;
    } else {
        return client->leaveRoom();
    }
}
QList<QString> NetworkManager::getRoomList() const {
    if(isServerMode) {
        return server->getRoomList();
    }
    return QList<QString>();
}

//------------------------------------ send_move ------------------------------------

bool NetworkManager::sendMove(const Move& move) {
    if(isServerMode) {
        return true;
    } else {
        return client->sendMove(move);
    }
}
bool NetworkManager::sendColorUpdate(const QString& color) {
    if (isServerMode) {
        return true;
    } else {
        QJsonObject msg;
        msg["type"] = "colorUpdate";
        msg["color"] = color;
        msg["roomId"] = client->getCurrentRoomId();
        msg["username"] = client->getUsername();
        return client->sendMessage(msg);
    }
}

bool NetworkManager::sendReadyStatus(bool ready) {
    if (isServerMode) {
        return true;
    } else {
        QJsonObject msg;
        msg["type"] = "ready";
        msg["ready"] = ready;
        msg["roomId"] = client->getCurrentRoomId();
        msg["username"] = client->getUsername();
        return client->sendMessage(msg);
    }
}






