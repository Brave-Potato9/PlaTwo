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