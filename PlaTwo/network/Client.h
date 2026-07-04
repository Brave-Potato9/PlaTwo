#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
#include <QTcpSocket>
#include "../core/Move.h"
class Client : public QObject
{
    Q_OBJECT
private:
    QTcpSocket * socket;
    QString currentRoomId;
    QString username;

    //helper_methods
    void handleMessage(const QJsonObject& message);
    void sendMessage(const QJsonObject& message);
public:
    //constructor_destructor
    explicit Client(QObject * parent = nullptr);
    ~Client();

    //connect_disconnect
    bool connectToServer(const QString& ip, int port);
    void disconnectFromServer();
    bool isConnected() const;

    //join_leave_room
    bool joinRoom(const QString& roomId,const QString& username);
    bool leaveRoom();

    //send_move
    bool sendMove(const Move& move);
signals:
    void connected();
    void disconnected();
    void connectionFailed(const QString& reason);
    void joinedRoom(const QString& roomId);
    void joinFailed(const QString& reason);
    void moveReceived(const Move& move);
    void playerJoined(const QString& username);
    void playerLeft(const QString& username);
    void gameStarted();
    void gameEnded(const QString winner);
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError();


};

#endif // CLIENT_H
