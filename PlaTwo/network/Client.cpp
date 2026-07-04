#include "Client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
//------------------------------------ constructor_destructor ------------------------------------

Client::Client(QObject * parent) : QObject(parent), socket(new QTcpSocket(this)) {
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &Client::onError);
}
Client::~Client() {
    disconnectFromServer();
}
//------------------------------------ connect_disconnect ------------------------------------
bool Client::connectToServer(const QString& ip, int port) {
    if(isConnected()) {
        disconnectFromServer();
    }
    socket->connectToHost(ip, port);
    return socket->waitForConnected(3000);
}
void Client::disconnectFromServer() {
    if(socket) {
        socket->disconnectFromHost();
        if(socket->state() == QAbstractSocket::UnconnectedState){
            socket->waitForDisconnected(1000);
        }
    }
}
bool Client::isConnected() const{
    return socket && socket->state() == QAbstractSocket::ConnectedState;
}
//------------------------------------ join_leave_room ------------------------------------
bool Client::joinRoom(const QString& roomId, const QString& username) {
    if(!isConnected()) return false;
    this->username = username;
    this->currentRoomId = roomId;
    QJsonObject message;
    message["type"] = "join";
    message["roomId"] = roomId;
    message["username"] = username;
    sendMessage(message);
    return true;
}
bool Client::leaveRoom() {
    if(currentRoomId.isEmpty()) return false;
    QJsonObject message;
    message["type"] = "leave";
    message["roomId"] = currentRoomId;
    sendMessage(message);
    currentRoomId.clear();
    return true;
}

//------------------------------------ send_move ------------------------------------

bool Client::sendMove(const Move& move) {
    if(!isConnected() || currentRoomId.isEmpty()) return false;
    QJsonObject message;
    message["type"] = "move";
    message["roomId"] = currentRoomId;
    message["move"] = move.toJson();
    sendMessage(message);
    return true;
}

//------------------------------------ slots ------------------------------------
void Client::onConnected() {
    qDebug() << "Connect to server";
    emit connected();
}
void Client::onDisconnected() {
    qDebug() << "Disconnect from server";
    emit disconnected();
}
void Client::onReadyRead() {
    QByteArray data = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject()) return;
    handleMessage(doc.object());
}
void Client::onError() {
    QString errorString = socket->errorString();
    qDebug() << "Socket error: " << errorString;
    emit connectionFailed(errorString);
}

//------------------------------------ helper_methods ------------------------------------
void Client::sendMessage(const QJsonObject& message) {
    if(socket && socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray data = QJsonDocument(message).toJson();
        socket->write(data);
        socket->flush();
    }
}
void Client::handleMessage(const QJsonObject& message) {
    QString type = message["type"].toString();
    if(type == "joinSuccess") {
        QString roomId = message["roomId"].toString();
        emit joinedRoom(roomId);
    } else if( type == "joinFailed") {
        QString reason = message["reason"].toString();
        emit joinFailed(reason);
    } else if(type == "move") {
        QJsonObject moveData = message["move"].toObject();
        Move move = Move::fromJson(moveData);
        emit moveReceived(move);
    } else if(type == "playerJoined") {
        QString username = message["username"].toString();
        emit playerJoined(username);
    } else if (type == "PlayerLeft") {
        QString username = message["username"].toString();
        emit playerLeft(username);
    } else if(type == "gameStarted") {
        emit gameStarted();
    } else if(type == "gameEnded") {
        QString winner = message["winner"].toString();
        emit gameEnded(winner);
    }
}





