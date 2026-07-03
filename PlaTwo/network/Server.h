#ifndef SERVER_H
#define SERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "Room.h"
class Server : public QObject
{
    Q_OBJECT
private:
    QTcpServer * server;
    QMap<QTcpSocket *, QString> clients;
    QMap<QString, Room*> rooms;
    QMap<QTcpSocket* , QString> socketRoomMap; // we can find a socket faster and send to related room

    //helper_methods
    void broadcastToRoom(const QString& roomId, const QByteArray& message);
    void sendToClient(QTcpSocket * socket, const QByteArray& message);
    void handleMessage(QTcpSocket * socket, const QJsonObject& message);
public:
    //constructor_destructor
    explicit Server(QObject * parent = nullptr);
    ~Server();

    //start_stop_server
    bool startServer(int port);
    void stopServer();
    bool isRunning() const;
    int getPort() const;
    QString getIP() const;

    //manage_rooms
    bool createRoom(const QString& roomId, const GameConfig& config, const QString& host);
    bool joinRoom(const QString& roomId, const QString& username);
    bool leaveRoom(const QString& roomId, const QString& username);
    Room* getRoom(const QString& roomId) const;
    QList<QString> getRoomList() const;

signals:
    void clientConnected(const QString& username);
    void clientDisconnected(const QString& username);
    void roomCreated(const QString& roomId);
    void roomDestroyed(const QString& roomId);
    void gameStarted(const QString& roomId);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();
};

#endif // SERVER_H
