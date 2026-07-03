#include "GameHistory.h"

//------------------------------------ constructor ------------------------------------
GameHistory::GameHistory():
    scores(2, 0)
{}

GameHistory::GameHistory(const QStringList& players, const QString& gameType)
    : players(players)
    , gameType(gameType)
    , scores(2, 0)
{}
