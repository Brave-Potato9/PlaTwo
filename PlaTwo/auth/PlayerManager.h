#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H
#include <QList>
#include <QObject>
#include <QString>
#include "../core/Player.h"
class PlayerManager : public QObject
{
    Q_OBJECT
private:
    QList<Player> players;
    QString filePath;
public:
    //constructor
    explicit PlayerManager(QObject * parent = nullptr);

    //load_and_save_players
    bool loadPlayers();
    bool savePlayers();

    //add_remove_update_player
    bool addPlayer(const Player& player);
    bool removePlayer(const QString& username);
    bool updatePlayer(const Player& player);

    //check_Exists_and_find
    bool findPlayer(const QString& username, Player& foundedPlayer) const;
    bool playerExists(const QString& username);

    //get_all_and_active_players
    QList<Player>& getAllPlayers();
    QList<Player> getActivePlayers() const;

    //update_last_login
    void updateLastLogin(const QString& username);

signals:
    void playerAdded(const QString& username);
    void playerRemoved(const QString& username);
    void playerUpdated(const QString& username);

};

#endif // PLAYERMANAGER_H
