#include "GameSession.h"
#include "../network/Room.h"

//------------------------------------ constructors ------------------------------------
GameSession::GameSession(Game* game, Room* room, QObject* parent)
    : QObject(parent)
    , sessionState(SessionState::Idle)
    , game(game)
    , room(room)
    , currentPlayerIndex(0)
    , hasTimeLimit(false)
    , timeLimitPerPlayer(60)
{
    if (game)
    {
        connect(game, &Game::gameLogicOver, this, &GameSession::onGameOver);
        connect(game, &Game::moveExecuted, this, &GameSession::onMoveExecuted);
        connect(game, &Game::scoreUpdated, this, [this](int player, int score)
            {
            emit gameStateChanged(QString("%1's score: %2").arg(player + 1).arg(score));
        });
    }

    connect(&timerManager, &TimerManager::timeExpired, this, &GameSession::onTimerExpired);
    connect(&timerManager, &TimerManager::timeUpdated, this, &GameSession::onTimerUpdated);

}

//------------------------------------ destructors ------------------------------------
GameSession::~GameSession()
{
    stopTimer();
}

//------------------------------------ session_management_methods ------------------------------------
bool GameSession::startSession(const GameConfig& config)
{
    if (!game || !room)
    {
        emit errorOccurred("Game or Room is null!");
        return false;
    }

    if (players.size() < 2)
    {
        emit errorOccurred("At least 2 players are required!");
        return false;
    }

    this->config = config;
    sessionState = SessionState::Starting;

    //initialize game with config
    initializeGame(config);

    //set the time
    hasTimeLimit = config.getHasTimeLimit();
    timeLimitPerPlayer = config.getTimeLimit();

    //start game
    game->setState(Game::State::Playing);
    game->setStartTime(QDateTime::currentDateTime());

    //start timer for first player
    if (hasTimeLimit)
    {
        startTimerForPlayer(0);
    }

    //change state and send signals
    sessionState = SessionState::Playing;
    emit sessionStarted();
    emit sessionStateChanged(sessionState);
    emit gameStateChanged("Game started!");

    //notify room
    if (room)
    {
        room->setGameState(Game::State::Playing);
    }

    return true;
}

void GameSession::endSession()
{
    if (sessionState == SessionState::GameOver || sessionState == SessionState::Aborted)
    {
        return;
    }

    stopTimer();
    sessionState = SessionState::GameOver;

    if (game && !game->isFinished())
    {
        game->setState(Game::State::GameOver);
        game->setEndTime(QDateTime::currentDateTime());
    }

    //save history
    saveHistory();

    //notify room
    if (room)
    {
        room->setGameState(Game::State::GameOver);
        broadcastGameEnded(game ? game->getWinner() : "Unknown");
    }

    emit sessionEnded(game ? game->getWinner() : "Unknown");
    emit sessionStateChanged(sessionState);
}

void GameSession::pauseSession()
{
    if (sessionState != SessionState::Playing)
    {
        return;
    }

    sessionState = SessionState::Paused;
    timerManager.pauseTimer(currentPlayerIndex);

    if (game)
    {
        game->setState(Game::State::Paused);
    }

    emit sessionPaused();
    emit sessionStateChanged(sessionState);
    emit gameStateChanged("Game paused!");
}

void GameSession::resumeSession()
{
    if (sessionState != SessionState::Paused)
    {
        return;
    }

    sessionState = SessionState::Playing;
    timerManager.resumeTimer(currentPlayerIndex);
    if (game)
    {
        game->setState(Game::State::Playing);
    }

    emit sessionResumed();
    emit sessionStateChanged(sessionState);
    emit gameStateChanged("Game continue!");
}

void GameSession::abortSession()
{
    stopTimer();
    sessionState = SessionState::Aborted;
    if (game)
    {
        game->setState(Game::State::Aborted);
    }

    emit sessionAborted();
    emit sessionStateChanged(sessionState);
    emit gameStateChanged("game cancled!");
}

//------------------------------------ player_management_methods ------------------------------------
void GameSession::addPlayer(const QString& username)
{
    if (players.contains(username)) return;
    if (players.size() >= 2)
    {
        emit errorOccurred("Room is full! (Max 2 players)");
        return;
    }

    players.append(username);

    emit playerJoined(username);
    emit gameStateChanged(QString("player %1 joined the session!").arg(username));
}

void GameSession::removePlayer(const QString& username)
{
    if (!players.contains(username)) return;

    players.removeAll(username);

    emit playerLeft(username);
    emit gameStateChanged(QString("player %1 left the session!").arg(username));
    qDebug() << "Player left:" << username;

    // if it less than 2 player pause the game
    if (players.size() < 2 && (sessionState == SessionState::Playing || sessionState == SessionState::Paused)) {
        abortSession();
    }
}

bool GameSession::isPlayerInSession(const QString& username) const
{
    return players.contains(username);
}

int GameSession::getPlayerCount() const
{
    return players.size();
}

QList<QString> GameSession::getPlayers() const
{
    return players;
}

//------------------------------------ turn_management_methods ------------------------------------
bool GameSession::makeMove(const Move& move)
{
    if (!game || sessionState != SessionState::Playing)
    {
        emit moveFailed(move, "Game is not running!");
        return false;
    }

    //is the moved players turn
    if (!isPlayerTurn(move.getPlayerUsername()))
    {
        emit moveFailed(move, "Not your turn!");
        return false;
    }

    //validaiting move
    if (!game->isValidMove(move))
    {
        emit moveFailed(move, "Invalid move!");
        emit gameStateChanged("Invalid move!");
        return false;
    }

    //execute move
    if (!game->makeMove(move))
    {
        emit moveFailed(move, "Error executing move!");
        emit gameStateChanged("Error executing move!");
        return false;
    }

    emit moveProcessed(move, true);
    emit gameStateChanged(QString("Move by %1").arg(move.getPlayerUsername()));

    //broadcast the move for all players
    broadcastMove(move);

    //check game is ended
    if (checkGameEnd())
    {
        return true;
    }

    //switch turn
    switchTurn();
    return true;
}

void GameSession::switchTurn()
{
    if (players.size() < 2)
    {
        return;
    }

    //change the index
    currentPlayerIndex = (currentPlayerIndex == 0) ? 1 : 0;

    //change the timer
    if (hasTimeLimit)
    {
        stopTimer();
        startTimerForPlayer(currentPlayerIndex);
    }

    QString playerName = getCurrentPlayer();
    emit turnChanged(currentPlayerIndex, playerName);
    emit gameStateChanged(QString("Turn: %1").arg(playerName));
}

bool GameSession::isPlayerTurn(const QString& username) const
{
    int idx = players.indexOf(username);
    if (idx == -1)
    {
        return false;
    }

    return idx == currentPlayerIndex;
}

QString GameSession::getCurrentPlayer() const
{
    if (currentPlayerIndex >= 0 && currentPlayerIndex < players.size())
    {
        return players[currentPlayerIndex];
    }

    return "";
}

int GameSession::getCurrentPlayerIndex() const
{
    return currentPlayerIndex;
}

void GameSession::setCurrentPlayer(int index)
{
    if (index < 0 || index >= players.size())
    {
        return;
    }

    currentPlayerIndex = index;
}

//------------------------------------ time_management_methods ------------------------------------
void GameSession::setTimeLimit(int seconds)
{
    timeLimitPerPlayer = seconds;
    hasTimeLimit = (seconds > 0);
}

int GameSession::getRemainingTime(int playerIndex) const
{
    return timerManager.getRemainingTime(playerIndex);
}

void GameSession::startTimerForPlayer(int playerIndex)
{
    if (!hasTimeLimit)
    {
        return;
    }

    int remaining = timerManager.getRemainingTime(playerIndex);
    if (remaining <= 0)
    {
        remaining = timeLimitPerPlayer;
    }

    timerManager.startTimer(playerIndex, remaining);
}

void GameSession::stopTimer()
{
    timerManager.stopTimer(0);
    timerManager.stopTimer(1);
}

//------------------------------------ state_management_methods ------------------------------------
GameSession::SessionState GameSession::getSessionState() const
{
    return sessionState;
}

bool GameSession::isActive() const
{
    return sessionState == SessionState::Playing;
}

bool GameSession::isFinished() const
{
    return (sessionState == SessionState::GameOver || sessionState == SessionState::Aborted);
}

QString GameSession::getStateString() const
{
    switch (sessionState)
    {
    case SessionState::Idle:
        return "Idle";
    case SessionState::WaitingForPlayers:
        return "WaitingForPlayers";
    case SessionState::Starting:
        return "Starting";
    case SessionState::Playing:
        return "Playing";
    case SessionState::Paused:
        return "Paused";
    case SessionState::GameOver:
        return "GameOver";
    case SessionState::Aborted:
        return "Aborted";
    default:
        return "Unknown";
    }
}