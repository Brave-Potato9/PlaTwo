#ifndef GAMEROOMWINDOW_H
#define GAMEROOMWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QDateTime>
#include <QTimer>
#include <QUuid>
#include "../auth/AuthManager.h"
#include "../core/Game.h"
#include "../core/GameSession.h"
#include "../core/GameHistoryManager.h"
#include "../network/NetworkManager.h"
#include "../network/Room.h"
#include "../games/dotsandboxes/DotsAndBoxesGame.h"
#include "../games/morris/MorrisGame.h"
#include "../games/fanorona/FanoronaGame.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameRoomWindow; }
QT_END_NAMESPACE

class GameRoomWindow : public QMainWindow
{
    Q_OBJECT

public:
    // constructor_destructor
    explicit GameRoomWindow(Room* room,
                            const QString& gameType,
                            const GameConfig& config,
                            AuthManager* authManager,
                            const QString& username,
                            const QColor& playerColor,
                            NetworkManager* networkManager,
                            bool isHost,
                            const QString& roomId = "",
                            QWidget* parent = nullptr);
    ~GameRoomWindow();
    // manage_view_status
    void updateStatusBar(const QString& status, const QString& color = "#27ae60");
    bool loadSavedGame(const QString& saveFile);



signals:
    void closed();
    void gameFinished(const QString& winner);

private slots:
    // buttons
    void onStartGameClicked();
    void onPauseGameClicked();
    void onResumeGameClicked();
    void onSaveGameClicked();
    void onBackClicked();
    void onReadyClicked();
    // void onSendChatClicked();

    // game_events
    void onGameStarted();
    void onGameEnded(const QString& winner);
    void onTurnChanged(int playerIndex, const QString& username);
    void onMoveMade(const Move& move);
    void onScoreUpdated(int playerIndex, int score);
    void onTimeUpdated(int playerIndex, int seconds);
    void onTimeExpired(int playerIndex, const QString& username);
    void onInvalidMove(const Move& move, const QString& reason);
    void onSessionEnded(const QString& winner);
    void onSessionAborted();
    void onBoardMoveSelected(const Move& move);


    // network_events
    void onMoveReceived(const Move& move);
    void onPlayerJoined(const QString& username);
    void onPlayerLeft(const QString& username);
    void onConnectedToServer();
    void onDisconnectedFromServer();
    void onConnectionFailed(const QString& reason);
    void onGameStartedFromServer();
    void onGameEndedFromServer(const QString& winner);
    void onPlayerReadyChanged(const QString& username, bool ready);
    void onColorUpdateReceived(const QString& color);

    // Room_events
    void onRoomPlayerJoined(const QString& username);
    void onRoomPlayerLeft(const QString& username);
    void onRoomGameStarted();
    void onRoomGameEnded(const QString& winner);
    void onRoomMessageReceived(const QByteArray& message);


private:
    Ui::GameRoomWindow *ui;

    Room* room;
    QString m_gameType;
    GameConfig m_config;
    QString m_username;
    QString m_opponentName;
    QString m_roomId;
    bool m_isHost;
    bool m_isReady;

    // manager
    AuthManager* m_authManager;
    NetworkManager* m_networkManager;
    Game* m_game;
    GameSession* m_session;
    GameHistoryManager m_historyManager;
    QString saveFilePath;

    // game status
    bool m_isPaused;
    bool m_isGameOver;
    bool m_isGameStarted;
    QDateTime m_startTime;
    int m_playerScore;
    int m_opponentScore;
    QTimer* m_autoSaveTimer;
    QTimer* m_syncTimer;
    QTimer* m_statusTimer;

    // players status
    bool m_player1Ready;
    bool m_player2Ready;
    QString m_player1Name;
    QString m_player2Name;
    QColor m_player1Color;
    QColor m_player2Color;
    QColor m_playerColor;
    QColor m_opponentColor;

    // connection_status
    bool m_isConnected;
    bool m_isWaitingForOpponent;

    // setupUI_and_connections
    void setupUI();
    void setupConnections();
    void setupRoomConnections();
    bool initializeGameObjects();
    void setupPlayers();
    void initBoard();
    void updateUI();
    // manage_view_status
    void updateScoreDisplay();
    void updateTimeDisplay(int playerIndex, int seconds);
    void updateTurnDisplay(int playerIndex);
    void updateButtons();
    void showGameOverDialog(const QString& winner);
    void syncPlayerStatus();
    // helper_methods
    void updatePlayerList();
    void saveGameHistory(const QString& winner);
    void resetGameState();
    void setControlsEnabled(bool enabled);
    void checkAndStartGame();
    void requestNewColor();
    void applyPlayerColors();
    bool isColorDuplicate(const QColor& color);
    void broadcastGameState();
    void syncGameState();
    void syncPlayersFromRoom();
    void sendColorUpdate();
    void handleColorUpdate(const QJsonObject& data);
    void startGameUI();
    // void sendChatMessage(const QString& message);
    void displayChatMessage(const QString& sender, const QString& message);

    // save_and_load
    void autoSaveGame();
    QString getSaveFilePath() const;
    QJsonObject createSaveData() const;
    void restoreFromSaveData(const QJsonObject& saveData);

    // send_information_to_network
    void sendMoveToNetwork(const Move& move);
    void sendReadyStatus();
    void sendGameState();
    void sendPlayerStatus();
};

#endif // GAMEROOMWINDOW_H