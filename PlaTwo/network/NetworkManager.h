#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <QObject>
#include "Server.h"
#include "Client.h"
class NetworkManager : public QObject
{
    Q_OBJECT
private:
    bool isServerMode;
    Client * client;
    Server* server;
public:
    //constructor
    explicit NetworkManager(QObject*  parent = nullptr );

    //manage_server
    bool startServer(int port);
    void stopServer();
    bool isServerRunning() const;
    QString getServerIP() const;

    //manage_client
    bool connectToServer(const QString& ip, int port);
    void disconnectFromServer();
    bool isClientConnected() const;

    //manage_rooms
    bool createRoom(const QString& roomId, const GameConfig& config, const QString& host);
    bool joinRoom(const QString roomId, const QString& username);
    bool leaveRoom();
    QList<QString> getRoomList() const;

    //manage_moves
    bool sendMove(const Move& move);
signals:
    //server-signals
    void clientConnected(const QString& username);
    void clientDisconnected(const QString& username);
    void roomCreated(const QString& roomId);
    void roomDestroyed(const QString& roomId);

    //client_signals
    void connectedToServer();
    void disconnectedFromServer();
    void connectionFailed(const QString& reason);
    void joinedRoom(const QString& roomId);
    void joinFailed(const QString& reason );
    void moveReceived(const Move& move);
    void playerJoinedRoom(const QString& username);
    void playerLeftRoom(const QString& username);
    void gameStarted();
    void gameEnded(const QString& winner);


};

#endif // NETWORKMANAGER_H
