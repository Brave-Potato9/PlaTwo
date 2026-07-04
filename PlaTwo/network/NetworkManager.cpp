#include "NetworkManager.h"
//------------------------------------ constructor ------------------------------------

NetworkManager::NetworkManager(QObject * parent) :QObject(parent)
    ,isServerMode(false)
    ,client(new Client(this))
    ,server(new Server(this)){

    //connect_server_signals
    connect(server, &Server::clientConnected, this, &NetworkManager::clientConnected);
    connect(server, &Server::clientConnected, this, &NetworkManager::clientDisconnected);
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
    connect(client, &Client::gameStarted, this, &NetworkManager::gameEnded);

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
    return server->isRunning();
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






