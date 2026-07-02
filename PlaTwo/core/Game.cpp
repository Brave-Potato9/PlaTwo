#include "Game.h"

//------------------------------------ constructors ------------------------------------
Game::Game(QObject* parent)
    : QObject(parent)
    , state(State::Idle)
    , isGameOver(false)
    , scores(2, 0)
{}


