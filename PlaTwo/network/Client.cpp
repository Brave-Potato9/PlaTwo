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
    if(!socket) return false;

    if(isConnected()) {
        disconnectFromServer();
    }

    socket->connectToHost(ip, port);
    return true;
}

void Client::disconnectFromServer()
{
    if(socket) {
        if(socket->state() == QAbstractSocket::ConnectedState ||
            socket->state() == QAbstractSocket::ConnectingState) {
            socket->disconnectFromHost();
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
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        if(line.isEmpty()) continue;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);

        if(parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "Client JSON Parse Error:" << parseError.errorString();
            continue;
        }

        handleMessage(doc.object());
    }
}
void Client::onError() {
    QString errorString = socket->errorString();
    qDebug() << "Socket error: " << errorString;
    emit connectionFailed(errorString);
}

//------------------------------------ helper_methods ------------------------------------
bool Client::sendMessage(const QJsonObject& message)
{
    if(!isConnected())
    {
        qDebug()<<"Socket not connected";
        return false;
    }


    QByteArray data =
        QJsonDocument(message).toJson(QJsonDocument::Compact);


    data.append('\n');


    qDebug()
        <<"Sending:"
        <<data;


    socket->write(data);
    socket->flush();


    return true;
}

void Client::handleMessage(const QJsonObject& message) {
    QString type = message["type"].toString();
    if(type == "joinSuccess") {
        QString roomId = message["roomId"].toString();
        GameConfig config;
        if (message.contains("config")) {
            QJsonObject configObj = message["config"].toObject();
            config = GameConfig::fromJson(configObj);
        }
        emit joinedRoom(roomId, config);
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
    } else if (type == "playerLeft") {
        QString username = message["username"].toString();
        emit playerLeft(username);
    } else if(type == "gameStarted") {
        emit gameStarted();
    } else if(type == "gameEnded") {
        QString winner = message["winner"].toString();
        emit gameEnded(winner);
    } else if (type == "colorUpdate") {
        QString username = message["username"].toString();
        QString color = message["color"].toString();
        emit colorUpdated(username, color);
    } else if(type == "ready") {
        QString username = message["username"].toString();
        bool ready = message["ready"].toBool();
        emit playerReadyChanged(username, ready);
    } else if (type == "boardState") {
        QJsonObject boardState = message["boardState"].toObject();
        emit boardStateReceived(boardState);
    } else if (type == "gamePaused") {
        emit gamePaused();
    } else if (type == "gameResumed") {
        emit gameResumed();
    } else if (type == "syncRequest") {
        emit syncRequested();
    }
}





