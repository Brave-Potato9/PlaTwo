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

