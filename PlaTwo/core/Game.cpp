#include "Game.h"

//------------------------------------ constructors ------------------------------------
Game::Game(QObject* parent)
    : QObject(parent)
    , state(State::Idle)
    , isGameOver(false)
    , scores(2, 0)
{}

//------------------------------------ constructors ------------------------------------
Game::~Game(){}

//------------------------------------ score_management_methods ------------------------------------
QVector<int> Game::getScores() const
{
    return scores;
}

int Game::getScore(int playerIndex) const
{
    if (playerIndex < 0 || playerIndex >= scores.size())
    {
        return 0;
    }

    return scores[playerIndex];
}

void Game::addScore(int playerIndex, int points)
{
    if (playerIndex < 0 || playerIndex >= scores.size())
    {
        return;
    }

    scores[playerIndex] += points;
    emit scoreUpdated(playerIndex, scores[playerIndex]);
    emit scoresUpdated(scores);
}

void Game::setScore(int playerIndex, int points)
{
    if (playerIndex < 0 || playerIndex >= scores.size())
    {
        return;
    }

    scores[playerIndex] = points;
    emit scoreUpdated(playerIndex, scores[playerIndex]);
    emit scoresUpdated(scores);
}

void Game::clearScores()
{
    scores[0] = 0;
    scores[1] = 0;
    emit scoresUpdated(scores);
}

//------------------------------------ state_management_methods ------------------------------------
Game::State Game::getState() const
{
    return state;
}

void Game::setState(State newState)
{
    if (state == newState)
    {
        return;
    }

    state = newState;
    emit stateChanged(state);

    if (state == State::GameOver)
    {
        onGameEnd(winner);
    }
}

bool Game::isFinished() const
{
    return state == State::GameOver || state == State::Aborted;
}

bool Game::isPlaying() const
{
    return state == State::Playing;
}

QString Game::getStateString() const
{
    switch (state) {
    case State::Idle:
        return "Idle";
    case State::Playing:
        return "Playing";
    case State::Paused:
        return "Paused";
    case State::GameOver:
        return "GameOver";
    case State::Aborted:
        return "Aborted";
    default:
        return "Unknown";
    }
}

//------------------------------------ history_management_methods ------------------------------------
QList<Move> Game::getMoveHistory() const
{
    return moveHistory;
}

void Game::addMoveToHistory(const Move& move)
{
    moveHistory.append(move);
    emit moveExecuted(move);
}

int Game::getMoveCount() const
{
    return moveHistory.size();
}

void Game::clearMoveHistory()
{
    moveHistory.clear();
}

//------------------------------------ time_management_methods ------------------------------------
QDateTime Game::getStartTime() const
{
    return startTime;
}

QDateTime Game::getEndTime() const
{
    return endTime;
}

void Game::setStartTime(const QDateTime& time)
{
    startTime = time;
}

void Game::setEndTime(const QDateTime& time)
{
    endTime = time;
}

qint64 Game::getDuration() const
{
    if (!startTime.isValid() || !endTime.isValid())
    {
        return 0;
    }

    return startTime.secsTo(endTime);
}