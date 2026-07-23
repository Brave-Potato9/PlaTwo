#include "GameRoomWindow.h"
#include "ui_GameRoomWindow.h"
#include <QMessageBox>
#include <QColorDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>
GameRoomWindow::GameRoomWindow(Room * _room ,const QString& gameType,
                        const GameConfig& config,
                        AuthManager* authManager,
                        const QString& username,
                        const QColor& playerColor,
                        NetworkManager* networkManager,
                        bool isHost,
                        const QString& roomId ,
                        QWidget* parent )
    :QMainWindow(parent)
    , ui(new Ui::GameRoomWindow)
    , room(_room)
    , m_gameType(gameType)
    , m_config(config)
    , m_username(username)
    , m_roomId(roomId)
    , m_isHost(isHost)
    , m_isReady(false)
    , m_authManager(authManager)
    , m_networkManager(networkManager)
    , m_game(nullptr)
    , m_session(nullptr)
    , m_isPaused(false)
    , m_isGameOver(false)
    , m_isGameStarted(false)
    , m_playerScore(0)
    , m_opponentScore(0)
    , m_autoSaveTimer(nullptr)
    , m_syncTimer(nullptr)
    , m_statusTimer(nullptr)
    , m_player1Ready(false)
    , m_player2Ready(false)
    , m_playerColor(playerColor)
{

    ui->setupUi(this);
    if (!initializeGameObjects()) {
        QMessageBox::critical(this, "Error",
                              "Failed to initialize game session!\n"
                              "Please try again.");
        QTimer::singleShot(0, this, &GameRoomWindow::close);
        return;
    }
    m_player1Name = m_username;
    m_player2Name = "Opponent";
    m_player1Color = m_playerColor;

    int r = 255 - m_playerColor.red();
    int g = 255 - m_playerColor.green();
    int b = 255 - m_playerColor.blue();
    m_opponentColor = QColor(r, g, b);
    m_player2Color = m_opponentColor;

    if(m_isHost) {
        m_opponentName = "Waiting for player ...";
        m_player2Color = Qt::gray;
    }
    syncPlayersFromRoom();
    setupUI();
    setupConnections();
    setupRoomConnections();
    setupPlayers();
    initBoard();
    updateUI();
    ui->pushButtonSaveGame->setVisible(false);
    if (!m_isHost) {
        ui->labelStatus->setText("⏳ Waiting for host to start...");
        updateStatusBar("Waiting for host...", "#f39c12");
    } else {
        updateStatusBar("Waiting for players...", "#f39c12");
    }

    m_statusTimer = new QTimer(this);
    m_statusTimer->setInterval(5000);
    connect(m_statusTimer, &QTimer::timeout, this, &GameRoomWindow::syncPlayerStatus);
    m_statusTimer->start();

    ui->pushButtonReady->setVisible(true);
    if (m_isHost) {
        ui->pushButtonReady->setText("🟢 I'm Ready");
    } else {
        ui->pushButtonReady->setText("🟢 Ready");
    }
}
GameRoomWindow::~GameRoomWindow() {
    if(!m_isGameOver && m_session && m_session->isActive()) {
        autoSaveGame();
    }
    if(m_autoSaveTimer) {
        m_autoSaveTimer->stop();
        delete m_autoSaveTimer;
    }
    if(m_syncTimer) {
        m_syncTimer->stop();
        delete m_syncTimer;
    }
    if(m_statusTimer) {
        m_statusTimer->stop();
        delete m_statusTimer;
    }
    delete ui;
}
bool GameRoomWindow::initializeGameObjects()
{
    if (!room) {
        qDebug() << "Room is null!";
        return false;
    }

    m_session = room->getGameSession();
    if (!m_session) {
        qDebug() << "GameSession is null!";
        return false;
    }

    m_game = m_session->getGame();
    if (!m_game) {
        qDebug() << "Game is null!";
        return false;
    }

    return true;
}
void GameRoomWindow::setupUI() {
    setWindowTitle("Game Room - " + m_gameType);
    setMinimumSize(800,650);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint );

    ui->pushButtonStartGame->setEnabled(false);
    ui->pushButtonPauseGame->setEnabled(false);
    ui->pushButtonResumeGame->setEnabled(false);
    ui->pushButtonSaveGame->setEnabled(false);
    ui->pushButtonReady->setVisible(true);
    QString displayName = m_gameType;
    if(displayName == "DotsAndBoxes") displayName = "Dots and Boxes";
    if(displayName == "Morris") displayName = "Nine Men's Morris";
    else if(displayName == "Fanorona") displayName = "Fanorona";
    ui->labelGameTitle->setText(displayName);

    applyPlayerColors();
}
void GameRoomWindow::setupConnections() {
    connect(ui->pushButtonStartGame, &QPushButton::clicked, this, &GameRoomWindow::onStartGameClicked);
    connect(ui->pushButtonPauseGame, &QPushButton::clicked, this, &GameRoomWindow::onPauseGameClicked);
    connect(ui->pushButtonResumeGame, &QPushButton::clicked, this, &GameRoomWindow::onResumeGameClicked);
    connect(ui->pushButtonSaveGame, &QPushButton::clicked, this, &GameRoomWindow::onSaveGameClicked);
    connect(ui->pushButtonBack, &QPushButton::clicked, this, &GameRoomWindow::onBackClicked);
    connect(ui->pushButtonReady, &QPushButton::clicked, this, &GameRoomWindow::onReadyClicked);

    if (m_game) {
        connect(m_game, &Game::moveExecuted, this, &GameRoomWindow::onMoveMade);
        connect(m_game, &Game::gameLogicOver, this, &GameRoomWindow::onGameEnded);
        connect(m_game, &Game::scoreUpdated, this, &GameRoomWindow::onScoreUpdated);
        connect(m_game, &Game::invalidMoveAttempted, this, &GameRoomWindow::onInvalidMove);
    }

    if (m_session) {
        connect(m_session, &GameSession::sessionStarted, this, &GameRoomWindow::onGameStarted);
        connect(m_session, &GameSession::turnChanged, this, &GameRoomWindow::onTurnChanged);
        connect(m_session, &GameSession::timeUpdated, this, &GameRoomWindow::onTimeUpdated);
        connect(m_session, &GameSession::timeExpired, this, &GameRoomWindow::onTimeExpired);
        connect(m_session, &GameSession::sessionEnded, this, &GameRoomWindow::onSessionEnded);
        connect(m_session, &GameSession::sessionAborted, this, &GameRoomWindow::onSessionAborted);
    }
    if(m_networkManager) {
        connect(m_networkManager, &NetworkManager::moveReceived, this, &GameRoomWindow::onMoveReceived);
        connect(m_networkManager, &NetworkManager::playerJoinedRoom, this, &GameRoomWindow::onPlayerJoined);
        connect(m_networkManager, &NetworkManager::playerLeftRoom, this, &GameRoomWindow::onPlayerLeft);
        connect(m_networkManager, &NetworkManager::connectedToServer, this, &GameRoomWindow::onConnectedToServer);
        connect(m_networkManager, &NetworkManager::disconnectedFromServer, this, &GameRoomWindow::onDisconnectedFromServer);
        connect(m_networkManager, &NetworkManager::connectionFailed, this, &GameRoomWindow::onConnectionFailed);
        connect(m_networkManager, &NetworkManager::gameStarted, this, &GameRoomWindow::onGameStartedFromServer);
        connect(m_networkManager, &NetworkManager::gameEnded, this, &GameRoomWindow::onGameEndedFromServer);
    }
    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setInterval(5000);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &GameRoomWindow::autoSaveGame);

    m_syncTimer = new QTimer(this);
    m_syncTimer->setInterval(2000);
    connect(m_syncTimer, &QTimer::timeout, this, &GameRoomWindow::syncGameState);

}
void GameRoomWindow::setupRoomConnections() {
    if(!room) {
        return;
    }
    connect(room, &Room::playerJoined, this, &GameRoomWindow::onRoomPlayerJoined);
    connect(room, &Room::playerLeft, this, &GameRoomWindow::onRoomPlayerLeft);
    connect(room, &Room::gameStarted, this, &GameRoomWindow::onRoomGameStarted);
    connect(room, &Room::gameEnded, this, &GameRoomWindow::onRoomGameEnded);
    connect(room, &Room::messageToAll, this, &GameRoomWindow::onRoomMessageReceived);
}
void GameRoomWindow::setupPlayers() {
    ui->labelPlayer1Name->setText(m_username);
    ui->labelPlayer1Color->setStyleSheet(
        "background-color: " + m_player1Color.name() + ";"
        "border-radius: 11px;"
        "border: 2px solid white;"
        );
    ui->labelPlayer1Score->setText("⭐ 0");
    ui->labelPlayer1Time->setText("⏱ --:--");
    ui->labelPlayer1Status->setText("⏳ Waiting");

    ui->labelPlayer2Name->setText(m_opponentName);
    ui->labelPlayer2Color->setStyleSheet(
        "background-color: " + m_player2Color.name() + ";"
        "border-radius: 11px;"
        "border: 2px solid white;"
        );
    ui->labelPlayer2Score->setText("⭐ 0");
    ui->labelPlayer2Time->setText("⏱ --:--");
    ui->labelPlayer2Status->setText("⏳ Waiting");

    ui->labelTurn->setText("⏳ Waiting for players...");
    ui->labelTurn->setStyleSheet(
        "font-size: 16px;"
        "font-weight: bold;"
        "color: #f39c12"
        "background-color: rgba(243, 156, 18, 0.2);"
        "border-radius: 8px;"
        "padding: 8px;"
        );
    updatePlayerList();
}
void GameRoomWindow::applyPlayerColors() {
    ui->labelPlayer1Color->setStyleSheet(
        "background-color: " + m_player1Color.name() + ";"
        "border-radius: 11px;"
        "border: 2px solid white;"
        );
    ui->labelPlayer2Color->setStyleSheet(
        "background-color: " + m_player2Color.name() + ";"
        "border-radius: 11px;"
        "border: 2px solid white;"
        );
    if(ui->gameBoardWidget) {
        ui->gameBoardWidget->setPlayer1Color(m_player1Color);
        ui->gameBoardWidget->setPlayer2Color(m_player2Color);
    }
}
bool GameRoomWindow::isColorDuplicate(const QColor& color) {
    return (color == m_player1Color || color == m_player2Color);
}
void GameRoomWindow::requestNewColor() {
    QMessageBox::warning(this, "Color Conflict",
    "You seleted color is already taken by the opponent.\n"
                         "Please choose a different color.");
    QColor newColor = QColorDialog::getColor(Qt:: blue, this, "Select Different Color");
    if(!newColor.isValid()) {
        newColor = QColor(
            QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255));
    }
    while(isColorDuplicate(newColor)) {
        QMessageBox::warning(this, "Still Duplicate",
        "This color is still taken. Please choose another.");
        newColor = QColorDialog::getColor(Qt::blue, this, "Select Different Color");

        if(!newColor.isValid()) {
            newColor = QColor(
                QRandomGenerator::global()->bounded(255),
                QRandomGenerator::global()->bounded(255),
                QRandomGenerator::global()->bounded(255));
        }
    }
    if(m_isHost) {
        m_player2Color = newColor;

    } else {
        m_player1Color = newColor;

    }
    sendColorUpdate();
    applyPlayerColors();
    updateUI();
}
void GameRoomWindow::initBoard() {
    if(!ui->gameBoardWidget || !m_game) return;
    ui->gameBoardWidget->setGame(m_game);

    if (m_gameType == "DotsAndBoxes") {
        int size = m_config.getDotsAndBoxesRows();
        ui->gameBoardWidget->setBoardSize(size, size);
    } else if (m_gameType == "Fanorona") {
        ui->gameBoardWidget->setBoardSize(9, 5);
    } else {
        ui->gameBoardWidget->setBoardSize(7, 7);
    }
    applyPlayerColors();
}
void GameRoomWindow::onBoardMoveSelected(const Move& move)
{
    if (m_session && m_session->isActive() && !m_isPaused && !m_isGameOver) {

        if (m_session->isPlayerTurn(m_username)) {
            m_session->makeMove(move);
        } else {
            updateStatusBar("Not your turn!", "#e74c3c");
        }
    }
}
void GameRoomWindow::onReadyClicked() {
    if(m_isGameStarted || m_isGameOver) return;
    m_isReady = !m_isReady;

    if (m_isReady) {
        ui->pushButtonReady->setText("✅ Ready");
        ui->pushButtonReady->setStyleSheet(
            "QPushButton {"
            "    background-color: #27ae60;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "    padding: 8px 20px;"
            "}"
            );
        updateStatusBar("You are ready!", "#27ae60");
    } else {
        ui->pushButtonReady->setText("🟢 Ready");
        ui->pushButtonReady->setStyleSheet(
            "QPushButton {"
            "    background-color: #3498db;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 8px;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "    padding: 8px 20px;"
            "}"
            "QPushButton:hover { background-color: #2980b9; }"
            );
        updateStatusBar("Not ready", "#95a5a6");
    }
    sendReadyStatus();
    checkAndStartGame();
}
void GameRoomWindow::onStartGameClicked()
{
    if (m_isGameStarted || m_isGameOver) return;
    if (!room || !m_session) return;

    if (m_isHost) {
        m_session->startSession(m_config);
        room->setGameState(Game::State::Playing);
    } else {
        updateStatusBar("Waiting for host to start...", "#f39c12");
        return;
    }

    m_isGameStarted = true;
    m_startTime = QDateTime::currentDateTime();

    ui->pushButtonStartGame->setEnabled(false);
    ui->pushButtonPauseGame->setEnabled(true);
    ui->pushButtonSaveGame->setEnabled(true);
    ui->pushButtonReady->setEnabled(false);
    ui->pushButtonReady->setVisible(false);

    if (m_autoSaveTimer) {
        m_autoSaveTimer->start();
    }
    if (m_syncTimer) {
        m_syncTimer->start();
    }

    updateStatusBar("Game started!", "#27ae60");
}
void GameRoomWindow::onPauseGameClicked()
{
    if (!m_session || m_isPaused || m_isGameOver) return;

    m_session->pauseSession();
    m_isPaused = true;

    ui->pushButtonPauseGame->setEnabled(false);
    ui->pushButtonResumeGame->setEnabled(true);

    if (m_autoSaveTimer) {
        m_autoSaveTimer->stop();
    }

    if (room) {
        // room->sendGamePaused();
    }

    updateStatusBar("Game paused", "#f39c12");
}
void GameRoomWindow::onResumeGameClicked() {
    if (!m_session || !m_isPaused || m_isGameOver) return;

    m_session->resumeSession();
    m_isPaused = false;

    ui->pushButtonPauseGame->setEnabled(true);
    ui->pushButtonResumeGame->setEnabled(false);

    if (m_autoSaveTimer && m_isGameStarted) {
        m_autoSaveTimer->start();
    }

    if (room) {
        // room->sendGameResumed();
    }

    updateStatusBar("Game resumed", "#27ae60");
}
void GameRoomWindow::onSaveGameClicked() {
    autoSaveGame();
    QMessageBox::information(this, "Success", "Game saved successfully!");
}
void GameRoomWindow::onBackClicked()
{
    if (!m_isGameOver && m_session && m_session->isActive()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Exit Game",
                                      "Are you sure you want to exit the game?\n"
                                      "Your progress will be saved automatically.",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }

        autoSaveGame();
    }

    emit closed();
    this->close();
}
void GameRoomWindow::onGameStarted()
{
    m_isGameStarted = true;
    m_startTime = QDateTime::currentDateTime();

    ui->pushButtonStartGame->setEnabled(false);
    ui->pushButtonPauseGame->setEnabled(true);
    ui->pushButtonSaveGame->setEnabled(true);
    ui->pushButtonReady->setEnabled(false);
    ui->pushButtonReady->setVisible(false);

    updateStatusBar("Game started!", "#27ae60");
    ui->labelTurn->setText("🎯 Game in progress...");

    if (m_autoSaveTimer) {
        m_autoSaveTimer->start();
    }
    if (m_syncTimer) {
        m_syncTimer->start();
    }
}
void GameRoomWindow::onGameEnded(const QString& winner)
{
    if (m_isGameOver) return;

    m_isGameOver = true;
    m_isGameStarted = false;

    if (m_autoSaveTimer) {
        m_autoSaveTimer->stop();
    }
    if (m_syncTimer) {
        m_syncTimer->stop();
    }

    showGameOverDialog(winner);
    saveGameHistory(winner);
    setControlsEnabled(false);

    ui->pushButtonPauseGame->setEnabled(false);
    ui->pushButtonResumeGame->setEnabled(false);
    ui->pushButtonSaveGame->setEnabled(false);

    if (room) {
        room->notifyGameEnded(winner);
    }

    updateStatusBar("Game ended! Winner: " + winner, "#e74c3c");
    emit gameFinished(winner);
}
void GameRoomWindow::onSessionEnded(const QString& winner)
{
    qDebug() << "Session ended. Winner:" << winner;

    if (!m_isGameOver) {
        m_isGameOver = true;
        m_isGameStarted = false;

        saveGameHistory(winner);
        showGameOverDialog(winner);
        setControlsEnabled(false);

        ui->pushButtonPauseGame->setEnabled(false);
        ui->pushButtonResumeGame->setEnabled(false);
        ui->pushButtonSaveGame->setEnabled(false);

        if (m_autoSaveTimer) {
            m_autoSaveTimer->stop();
        }
        if (m_syncTimer) {
            m_syncTimer->stop();
        }

        updateStatusBar("Game ended. Winner: " + winner, "#e74c3c");

        if (room) {
            room->notifyGameEnded(winner);
        }

        emit gameFinished(winner);
    }
}

void GameRoomWindow::onSessionAborted() {
    qDebug() << "Session aborted!";

    if (!m_isGameOver) {
        m_isGameOver = true;
        m_isGameStarted = false;

        autoSaveGame();

        QMessageBox::warning(this, "Game Aborted",
                             "The game has been aborted.\n"
                             "Your progress has been saved.");

        setControlsEnabled(false);
        ui->pushButtonPauseGame->setEnabled(false);
        ui->pushButtonResumeGame->setEnabled(false);
        ui->pushButtonSaveGame->setEnabled(false);

        if (m_autoSaveTimer) {
            m_autoSaveTimer->stop();
        }
        if (m_syncTimer) {
            m_syncTimer->stop();
        }

        updateStatusBar("Game aborted!", "#e74c3c");
        emit gameFinished("");
    }
}
void GameRoomWindow::onTurnChanged(int playerIndex, const QString& username)
{
    updateTurnDisplay(playerIndex);

    if (m_game) {
        QList<Move> possibleMoves = m_game->getPossibleMoves();
        if (ui->gameBoardWidget) {
            ui->gameBoardWidget->highlightValidMoves(possibleMoves);
        }
    }
}

void GameRoomWindow::onMoveMade(const Move& move)
{
    sendMoveToNetwork(move);

    if (ui->gameBoardWidget) {
        ui->gameBoardWidget->highlightLastMove(move);
    }
}
void GameRoomWindow::onScoreUpdated(int playerIndex, int score)
{
    if (playerIndex == 0) {
        m_playerScore = score;
        ui->labelPlayer1Score->setText("⭐ " + QString::number(score));
    } else {
        m_opponentScore = score;
        ui->labelPlayer2Score->setText("⭐ " + QString::number(score));
    }
}
void GameRoomWindow::onTimeUpdated(int playerIndex, int seconds)
{
    updateTimeDisplay(playerIndex, seconds);
}


void GameRoomWindow::onTimeExpired(int playerIndex, const QString& username)
{
    Q_UNUSED(username);
    QString playerName = (playerIndex == 0) ? m_player1Name : m_player2Name;
    QString message = QString("⏰ Time expired for %1!").arg(playerName);
    updateStatusBar(message, "#e74c3c");

    QString winner = (playerIndex == 0) ? m_player2Name : m_player1Name;
    onGameEnded(winner);
}
void GameRoomWindow::onInvalidMove(const Move& move, const QString& reason)
{
    Q_UNUSED(move);
    updateStatusBar("Invalid move! " + reason, "#e74c3c");
}
void GameRoomWindow::onMoveReceived(const Move& move)
{
    if (!m_game || m_isGameOver) return;

    if (move.getPlayerUsername() != m_username) {
        m_game->makeMove(move);
        if (ui->gameBoardWidget) {
            ui->gameBoardWidget->highlightLastMove(move);
        }
    }
}
void GameRoomWindow::onPlayerJoined(const QString& username) {
    QString message = QString("👋 %1 joined the room!").arg(username);
    updateStatusBar(message, "#3498db");

    if (room) {
        syncPlayersFromRoom();
        updatePlayerList();
    }
}
void GameRoomWindow::onPlayerLeft(const QString& username){
    QString message = QString("👋 %1 left the room.").arg(username);
    updateStatusBar(message, "#e74c3c");

    if (room) {
        syncPlayersFromRoom();
        updatePlayerList();
    }

    if (m_isGameStarted && !m_isGameOver) {
        if (m_session) {
            m_session->abortSession();
        }
        QMessageBox::information(this, "Game Aborted",
                                 "Opponent left the game.\nThe game has been aborted.");
        onGameEnded("");
    }
}
void GameRoomWindow::onConnectedToServer()
{
    m_isConnected = true;
    updateStatusBar("Connected to server", "#27ae60");
    displayChatMessage("System", "✅ Connected to server");
}
void GameRoomWindow::onDisconnectedFromServer()
{
    m_isConnected = false;
    updateStatusBar("Disconnected from server", "#e74c3c");

    if (m_isGameStarted && !m_isGameOver) {
        QMessageBox::warning(this, "Disconnected",
                             "You have been disconnected from the server.\n"
                             "The game will be saved and closed.");
        autoSaveGame();
        onGameEnded("");
    }
}
void GameRoomWindow::onConnectionFailed(const QString& reason)
{
    updateStatusBar("Connection failed: " + reason, "#e74c3c");
    QMessageBox::warning(this, "Connection Failed",
                         "Failed to connect to server:\n" + reason);
}
void GameRoomWindow::onGameStartedFromServer()
{
    if (!m_isHost && !m_isGameStarted) {
        onStartGameClicked();
    }
}
void GameRoomWindow::onGameEndedFromServer(const QString& winner)
{
    if (!m_isGameOver) {
        onGameEnded(winner);
    }
}
void GameRoomWindow::onRoomPlayerJoined(const QString& username)
{
    onPlayerJoined(username);
    syncPlayerStatus();
}
void GameRoomWindow::onRoomPlayerLeft(const QString& username)
{
    onPlayerLeft(username);
    syncPlayerStatus();
}
void GameRoomWindow::onRoomGameStarted()
{
    onGameStartedFromServer();
}

void GameRoomWindow::onRoomGameEnded(const QString& winner)
{
    onGameEndedFromServer(winner);
}
void GameRoomWindow::onRoomMessageReceived(const QByteArray& message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message);
    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();

    /*if (type == "chat") {
        QString sender = obj["sender"].toString();
        QString msg = obj["message"].toString();
        if (sender != m_username) {
            displayChatMessage(sender, msg);
        }
    }*/ if (type == "ready") {
        QString username = obj["username"].toString();
        bool ready = obj["ready"].toBool();
        if (username != m_username) {
            m_player2Ready = ready;
            updatePlayerList();
            checkAndStartGame();
        }
    } else if (type == "colorUpdate") {
        QString username = obj["username"].toString();
        QString color = obj["color"].toString();
        if (username != m_username) {
            QColor newColor(color);
            if (newColor.isValid()) {
                if (m_isHost) {
                    m_player2Color = newColor;
                } else {
                    m_player1Color = newColor;
                }
                applyPlayerColors();
                updateUI();
            }
        }
    } else if (type == "move") {
        QJsonObject moveData = obj["move"].toObject();
        Move move = Move::fromJson(moveData);
        onMoveReceived(move);
    } else if (type == "boardState") {
        if (ui->gameBoardWidget) {
            ui->gameBoardWidget->update();
        }
    } else if (type == "gamePaused") {
        if (!m_isPaused && !m_isGameOver) {
            m_isPaused = true;
            ui->pushButtonPauseGame->setEnabled(false);
            ui->pushButtonResumeGame->setEnabled(true);
            updateStatusBar("Game paused by opponent", "#f39c12");
            displayChatMessage("System", "Opponent paused the game");
        }
    } else if (type == "gameResumed") {
        if (m_isPaused && !m_isGameOver) {
            m_isPaused = false;
            ui->pushButtonPauseGame->setEnabled(true);
            ui->pushButtonResumeGame->setEnabled(false);
            updateStatusBar("Game resumed", "#27ae60");
            displayChatMessage("System", "Opponent resumed the game");
        }
    }
}
void GameRoomWindow::syncPlayersFromRoom() {
    if (!room) return;

    QStringList players = room->getPlayers();

    if (players.size() >= 1) {
        m_player1Name = players[0];
        if (m_player1Name == m_username) {
            m_player1Color = m_playerColor;
        } else {
            m_player2Name = m_player1Name;
            m_player1Name = m_username;
        }
    }

    if (players.size() >= 2) {
        m_player2Name = players[1];
        if (m_player2Name == m_username) {
            m_player2Name = players[0];
            m_player1Name = m_username;
        }
        ui->labelPlayer2Status->setText("✅ Connected");
        ui->labelPlayer2Status->setStyleSheet("color: #27ae60; font-size: 12px; font-weight: bold;");
        m_isWaitingForOpponent = false;
    } else {
        if (m_isHost) {
            ui->labelPlayer2Name->setText("Waiting for player...");
            ui->labelPlayer2Status->setText("⏳ Waiting");
            ui->labelPlayer2Status->setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold;");
            m_isWaitingForOpponent = true;
        }
    }

    updatePlayerList();
}
void GameRoomWindow::updatePlayerList() {
    if (!ui) return;

    ui->labelPlayer1Name->setText(m_player1Name);
    ui->labelPlayer2Name->setText(m_player2Name);

    if (m_player1Ready) {
        ui->labelPlayer1Status->setText("✅ Ready");
        ui->labelPlayer1Status->setStyleSheet("color: #27ae60; font-size: 12px; font-weight: bold;");
    } else {
        ui->labelPlayer1Status->setText("⏳ Waiting");
        ui->labelPlayer1Status->setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold;");
    }

    if (m_player2Name == "Waiting for player..." || m_player2Name == "Waiting for players...") {
        ui->labelPlayer2Status->setText("⏳ Waiting");
        ui->labelPlayer2Status->setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold;");
    } else if (m_player2Ready) {
        ui->labelPlayer2Status->setText("✅ Ready");
        ui->labelPlayer2Status->setStyleSheet("color: #27ae60; font-size: 12px; font-weight: bold;");
    } else {
        ui->labelPlayer2Status->setText("⏳ Not ready");
        ui->labelPlayer2Status->setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold;");
    }
}
void GameRoomWindow::updateUI() {
    updateScoreDisplay();
    updatePlayerList();
    updateStatusBar("");
    updateButtons();
}
void GameRoomWindow::updateScoreDisplay() {
    ui->labelPlayer1Score->setText("⭐ " + QString::number(m_playerScore));
    ui->labelPlayer2Score->setText("⭐ " + QString::number(m_opponentScore));
}
void GameRoomWindow::updateTimeDisplay(int playerIndex, int seconds) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    QString timeStr = QString("%1:%2")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(secs, 2, 10, QChar('0'));

    if (playerIndex == 0) {
        ui->labelPlayer1Time->setText("⏱ " + timeStr);
    } else {
        ui->labelPlayer2Time->setText("⏱ " + timeStr);
    }
}
void GameRoomWindow::updateTurnDisplay(int playerIndex) {
    QString playerName = (playerIndex == 0) ? m_player1Name : m_player2Name;
    QString turnText = QString("🎯 %1's Turn").arg(playerName);

    if (playerName == m_username) {
        turnText += " (You)";
        ui->labelTurn->setStyleSheet(
            "font-size: 16px;"
            "font-weight: bold;"
            "color: #27ae60;"
            "background-color: rgba(39, 174, 96, 0.2);"
            "border-radius: 8px;"
            "padding: 8px;"
            );
    } else {
        ui->labelTurn->setStyleSheet(
            "font-size: 16px;"
            "font-weight: bold;"
            "color: #e74c3c;"
            "background-color: rgba(231, 76, 60, 0.2);"
            "border-radius: 8px;"
            "padding: 8px;"
            );
    }

    ui->labelTurn->setText(turnText);
}
void GameRoomWindow::updateStatusBar(const QString& status, const QString& color) {
    if (!ui) return;

    if (!status.isEmpty()) {
        ui->labelStatus->setText(status);
    }

    if (!color.isEmpty()) {
        ui->labelStatus->setStyleSheet(
            "font-size: 12px;"
            "font-weight: bold;"
            "color: " + color + ";"
                      "background-color: rgba(0, 0, 0, 0.05);"
                      "border-radius: 8px;"
                      "padding: 5px 10px;"
            );
    }
}
void GameRoomWindow::updateButtons() {
    if (!ui) return;

    bool isGameActive = m_isGameStarted && !m_isGameOver;

    ui->pushButtonStartGame->setEnabled(
        m_isHost &&
        !m_isGameStarted &&
        !m_isGameOver &&
        m_player1Ready &&
        m_player2Ready
        );

    ui->pushButtonPauseGame->setEnabled(
        isGameActive &&
        !m_isPaused
        );

    ui->pushButtonResumeGame->setEnabled(
        isGameActive &&
        m_isPaused
        );

    ui->pushButtonSaveGame->setEnabled(
        isGameActive
        );

    ui->pushButtonReady->setEnabled(
        !m_isGameStarted &&
        !m_isGameOver
        );
}
void GameRoomWindow::syncPlayerStatus() {
    if (!room || !ui) return;

    QStringList players = room->getPlayers();
    int playerCount = players.size();

    if (playerCount >= 2) {
        m_isWaitingForOpponent = false;

        QString opponent = (players[0] == m_username) ? players[1] : players[0];
        if (m_player2Name != opponent) {
            m_player2Name = opponent;
            ui->labelPlayer2Name->setText(m_player2Name);
        }

        ui->labelPlayer2Status->setText("✅ Connected");
        ui->labelPlayer2Status->setStyleSheet("color: #27ae60; font-size: 12px; font-weight: bold;");

        ui->labelPlayer1Status->setText(m_isReady ? "✅ Ready" : "⏳ Not ready");
        ui->labelPlayer1Status->setStyleSheet(
            m_isReady ?
                "color: #27ae60; font-size: 12px; font-weight: bold;" :
                "color: #f39c12; font-size: 12px; font-weight: bold;"
            );

        if (m_isGameStarted && !m_isGameOver) {
            if (m_isPaused) {
                updateStatusBar("Game paused", "#f39c12");
            } else {
                updateStatusBar("Game in progress...", "#27ae60");
            }
            ui->labelTurn->setText("🎯 Game in progress...");
        } else if (m_isGameOver) {
            updateStatusBar("Game finished!", "#e74c3c");
        } else {
            if (m_player1Ready && m_player2Ready) {
                updateStatusBar("Both players ready! Starting...", "#27ae60");
                ui->labelTurn->setText("🎯 Starting game...");
            } else if (m_player1Ready) {
                updateStatusBar("Waiting for opponent to ready...", "#f39c12");
                ui->labelTurn->setText("⏳ Waiting for opponent...");
            } else if (m_player2Ready) {
                updateStatusBar("Press Ready to start!", "#3498db");
                ui->labelTurn->setText("⏳ Press Ready to start!");
            } else {
                if (m_isHost) {
                    updateStatusBar("Both players need to be ready!", "#f39c12");
                } else {
                    updateStatusBar("Press Ready to join!", "#3498db");
                }
                ui->labelTurn->setText("⏳ Waiting for players...");
            }
        }
    } else if (playerCount == 1) {
        m_isWaitingForOpponent = true;

        if (m_isHost) {
            ui->labelPlayer2Name->setText("Waiting for player...");
            ui->labelPlayer2Status->setText("⏳ Waiting");
            ui->labelPlayer2Status->setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold;");
            updateStatusBar("Waiting for opponent to join...", "#f39c12");
            ui->labelTurn->setText("⏳ Waiting for players...");
        } else {
            ui->labelPlayer1Name->setText("Host disconnected...");
            ui->labelPlayer1Status->setText("❌ Disconnected");
            ui->labelPlayer1Status->setStyleSheet("color: #e74c3c; font-size: 12px; font-weight: bold;");
            updateStatusBar("Host disconnected! Waiting...", "#e74c3c");
            ui->labelTurn->setText("❌ Host disconnected");
        }
    } else {
        m_isWaitingForOpponent = true;
        ui->labelPlayer2Name->setText("Waiting for players...");
        ui->labelPlayer2Status->setText("⏳ Waiting");
        ui->labelPlayer2Status->setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold;");
        updateStatusBar("Waiting for players...", "#f39c12");
        ui->labelTurn->setText("⏳ Waiting for players...");
    }

    applyPlayerColors();
    updateButtons();
}
void GameRoomWindow::syncGameState() {
    if (!m_game || !room || m_isGameOver) return;

    QJsonObject state;
    state["type"] = "boardState";
    state["board"] = m_game->getBoardState();
    state["currentPlayer"] = m_game->getCurrentPlayerIndex();

    QJsonArray scores;
    scores.append(m_game->getScore(0));
    scores.append(m_game->getScore(1));
    state["scores"] = scores;

    room->sendBoardState(state);
}
void GameRoomWindow::sendReadyStatus() {
    if(!room) return;
    room->sendReadyStatus(m_username, m_isReady);

    if (m_isReady) {
        if (m_isHost) {
            m_player1Ready = true;
        } else {
            m_player2Ready = true;
        }
    } else {
        if (m_isHost) {
            m_player1Ready = false;
        } else {
            m_player2Ready = false;
        }
    }
    updatePlayerList();
}
void GameRoomWindow::sendColorUpdate()
{
    if (!room) return;

    QString colorName = (m_isHost ? m_player2Color : m_player1Color).name();
    room->sendColorUpdate(m_username, colorName);
}
void GameRoomWindow::onPlayerReadyChanged(const QString& username, bool ready) {
    if(username == m_username) {
        m_player1Ready = ready;
    } else {
        m_player2Ready = ready;
    }
    if(username == m_player2Name) {
        ui->labelPlayer2Status->setText( ready ? "✅ Ready" : "⏳ Waiting");
        ui->labelPlayer2Status->setStyleSheet(
            ready ? "color: #27ae60; font-size: 12px; font-weight: bold;"
                  : "color: #f39c12; font-size: 12px; font-weight: bold;"
            );
    }
    checkAndStartGame();
}
void GameRoomWindow::checkAndStartGame() {
    if (m_player1Ready && m_player2Ready && !m_isGameStarted && !m_isGameOver) {
        updateStatusBar("Both players ready! Starting game...", "#27ae60");
        ui->labelTurn->setText("🎯 Game is starting...");

        QTimer::singleShot(1000, this, [this]() {
            if (!m_isGameStarted && !m_isGameOver) {
                onStartGameClicked();
            }
        });
    }

}
void GameRoomWindow::sendMoveToNetwork(const Move& move) {
    if (!room) return;

    QJsonObject msg;
    msg["type"] = "move";
    msg["move"] = move.toJson();
    msg["player"] = m_username;

    room->sendMessageToAll(msg);
}
void GameRoomWindow::autoSaveGame() {
    if (!m_game || m_isGameOver) return;

    QString filePath = getSaveFilePath();
    QJsonObject saveData = createSaveData();

    QDir dir("data/" + m_username + "/saves/");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(saveData).toJson());
        file.close();
    }
}
QString GameRoomWindow::getSaveFilePath() const {
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    return QString("data/%1/saves/%2_%3.json")
        .arg(m_username)
        .arg(m_gameType)
        .arg(timestamp);
}
QJsonObject GameRoomWindow::createSaveData() const {
    QJsonObject data;
    data["gameType"] = m_gameType;
    data["saveTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    data["player1"] = m_player1Name;
    data["player2"] = m_player2Name;
    data["currentPlayer"] = m_game ? m_game->getCurrentPlayerIndex() : 0;
    data["playerScore"] = m_playerScore;
    data["opponentScore"] = m_opponentScore;
    data["moveCount"] = m_game ? m_game->getMoveCount() : 0;
    data["isPaused"] = m_isPaused;
    data["playerColor"] = m_playerColor.name();

    if (m_game) {
        data["board"] = m_game->getBoardState();
        QJsonArray movesArray;
        for (const Move& move : m_game->getMoveHistory()) {
            movesArray.append(move.toJson());
        }
        data["moves"] = movesArray;
    }

    return data;
}
void GameRoomWindow::showGameOverDialog(const QString& winner) {
    QString message;
    if (winner.isEmpty()) {
        message = "Game Over!\nThe game was aborted.";
    } else if (winner == m_username) {
        message = "🎉 You Win!\nCongratulations!";
    } else if (winner == "Draw") {
        message = "⚖️ Game Draw!\nWell played!";
    } else {
        message = "😔 You Lose!\nBetter luck next time!";
    }

    QMessageBox::information(this, "Game Over", message);
}
void GameRoomWindow::saveGameHistory(const QString& winner) {
    if (!m_session) return;

    GameHistory history = m_session->getHistory();
    history.setWinner(winner);
    history.setEndTime(QDateTime::currentDateTime());

    m_historyManager.saveHistory(m_player1Name, m_gameType, history);
    if (m_player2Name != "Opponent" && m_player2Name != "Waiting for player...") {
        m_historyManager.saveHistory(m_player2Name, m_gameType, history);
    }
}
void GameRoomWindow::setControlsEnabled(bool enabled)
{
    ui->pushButtonPauseGame->setEnabled(enabled);
    ui->pushButtonResumeGame->setEnabled(false);
    ui->pushButtonSaveGame->setEnabled(enabled);
    ui->pushButtonReady->setEnabled(!enabled);
}
void GameRoomWindow::resetGameState()
{
    m_isGameStarted = false;
    m_isGameOver = false;
    m_isPaused = false;
    m_playerScore = 0;
    m_opponentScore = 0;
    m_player1Ready = false;
    m_player2Ready = false;
    m_isReady = false;

    if (m_game) {
        m_game->resetGame();
    }

    if (ui->gameBoardWidget) {
        ui->gameBoardWidget->clearHighlights();
        ui->gameBoardWidget->clearLastMove();
    }

    updateUI();
}
