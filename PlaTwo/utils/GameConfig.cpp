#include "GameConfig.h"

//------------------------------------ constructor ------------------------------------
GameConfig::GameConfig()
    : gameType("Unknown")
    , hasTimeLimit(false)
    , timeLimit(60)
    , serverPort(12345)
    , serverIP("127.0.0.1")
    , dotsAndBoxesRows(5)
    , dotsAndBoxesColumns(5)
    , useFlyingPhase(true)
{
    validateAndFix();
}

//------------------------------------ getters ------------------------------------
QString GameConfig::getGameType() const
{
    return gameType;
}

bool GameConfig::getHasTimeLimit() const
{
    return hasTimeLimit;
}

int GameConfig::getTimeLimit() const
{
    return timeLimit;
}

int GameConfig::getServerPort() const
{
    return serverPort;
}

QString GameConfig::getServerIP() const
{
    return serverIP;
}

int GameConfig::getDotsAndBoxesRows() const
{
    return dotsAndBoxesRows;
}

int GameConfig::getDotsAndBoxesColumns() const
{
    return dotsAndBoxesColumns;
}

bool GameConfig::getUseFlyingPhase() const
{
    return useFlyingPhase;
}
