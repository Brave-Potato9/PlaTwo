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
