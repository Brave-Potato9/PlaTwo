#include "Server.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
//#include <QNetworkInterface>
//------------------------------------ constructor_destructor ------------------------------------
Server::Server(QObject* parent ) : QObject(parent), server(new QTcpServer(this)) {}
Server::~Server() {
    stopServer();
    qDeleteAll(rooms);
    rooms.clear();
}

//------------------------------------ start_stop_server ------------------------------------

bool Server::startServer(int port) {
    if(server->listen(QHostAddress::LocalHost , port)) {
        connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);
        qDebug() << "Server started on port " << port;
        return true;
    }
    qDebug() << "Failed to start server: " << server->errorString();
    return false;
}
void Server::stopServer() {
    if(server->isListening()) {
        server->close();
    }
    for(auto socket : clients.keys()) {
        socket->disconnectFromHost();
    }
    clients.clear();
}
QString Server::getIP() const {
    /*for( const auto& interface : QNetworkInterface::allInterfaces()) {
        if(interface.flags().testFlags(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlags(QNetworkInterface::IsLoopBack)) {
            for (const auto& entry : interface.addressEntries()) {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    return entry.ip().toString();
                }
            }
        }
    }*/
    return "127.0.0.1";
}
int Server::getPort() const {
    return server->serverPort();
}

//------------------------------------ manage_rooms ------------------------------------
bool Server::createRoom(const QString& roomId, const GameConfig& config, const QString& host) {
    if(rooms.contains(roomId)) return false;
    Room * room = new Room(roomId, config, this);
    room->addPlayer(host);
    rooms[roomId] = room;
    connect(room , &Room::playerJoined, this, [this, roomId](const QString& username) {
        QJsonObject msg;
        msg["type"] = "playerJoined";
        msg["roomId"] = roomId;
        msg["username"] = username;
        broadcastToRoom(roomId, QJsonDocument(msg).toJson());
    });

    connect(room, &Room::gameStarted, this, [this, roomId]() {
        emit gameStarted(roomId);
        QJsonObject msg;
        msg["type"] = "gameStarted";
        msg["roomId"] = roomId;
        broadcastToRoom(roomId, QJsonDocument(msg).toJson());
    });

    connect(room, &Room::messageToAll, this, [this, roomId](const QByteArray& message) {
        broadcastToRoom(roomId, message);
    });

    emit roomCreated(roomId);
    return true;
}
bool Server::joinRoom(const QString& roomId, const QString& username){
    if(!rooms.contains(roomId)) return false;
    Room* room = rooms[roomId];
    bool success = room->addPlayer(username);
    if (success && room->isFull()) {
        room->startGame(room->getGameConfig());
    }
    return success;
}
bool Server::leaveRoom(const QString& roomId, const QString& username) {
    if(!rooms.contains(roomId)) return false;
    return rooms[roomId]->removePlayer(username);
}
Room* Server::getRoom(const QString& roomId) const {
    return rooms.value(roomId, nullptr);
}
QList<QString> Server::getRoomList() const {
    return rooms.keys();
}

//------------------------------------ helper_methods ------------------------------------

void Server::broadcastToRoom(const QString& roomId, const QByteArray& message) {
    for(auto it = socketRoomMap.begin() ; it != socketRoomMap.end() ; it++) {
        if(it.value() == roomId) {
            sendToClient(it.key(), message);
        }
    }
}
void Server::sendToClient(QTcpSocket * socket, const QByteArray& message) {
    if(socket && socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(message);
        socket->flush(); // send immediately
    }
}
void Server::handleMessage(QTcpSocket * socket, const QJsonObject& message) {
    QString type = message["type"].toString();
    if(type == "join") {
        QString username = message["username"].toString();
        QString roomId = message["roomId"].toString();

        if(joinRoom(roomId, username)) {
            clients[socket] = username;
            socketRoomMap[socket] = roomId;
            QJsonObject response;
            response["type"] = "joinSuccess";
            response["roomId"] = roomId;
            sendToClient(socket, QJsonDocument(response).toJson());
        } else {
            QJsonObject response;
            response["type"] = "joinFailed";
            response["reason"] = "Unable to join room";
            sendToClient(socket , QJsonDocument(response).toJson());
        }
    } else if( type == "move") {
        QString roomId = message["roomId"].toString();
        QJsonObject moveData = message["move"].toObject();
        Move move = Move::fromJson(moveData);
        Room* room = getRoom(roomId);
        if (room) {
            room->processMove(move);
        }

    }
}

//------------------------------------ slots ------------------------------------
void Server::onNewConnection() {
    QTcpSocket * socket = server->nextPendingConnection();
    if(!socket) return;
    connect(socket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);
    connect(socket , &QTcpSocket::readyRead, this, &Server::onReadyRead);
    qDebug() << "New client connected: " << socket->peerAddress().toString();
}
void Server::onClientDisconnected() {
    QTcpSocket * socket = qobject_cast<QTcpSocket*>(sender());
    if(!socket) return;
    QString username = clients.value(socket, "");
    if(!username.isEmpty()) {
        QString roomId = socketRoomMap.value(socket, "");
        if(!roomId.isEmpty()) {
            leaveRoom(roomId, username);
        }
        clients.remove(socket);
        socketRoomMap.remove(socket);
        emit clientConnected(username);
    }
    socket->deleteLater();
}
void Server::onReadyRead() {
    QTcpSocket * socket = qobject_cast<QTcpSocket*> (sender());
    if(!socket) return;
    QByteArray data = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject()) return;
    handleMessage(socket, doc.object());
}