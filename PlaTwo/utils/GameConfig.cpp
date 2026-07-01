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

//------------------------------------ setters ------------------------------------
void GameConfig::setGameType(const QString& _gameType)
{
    if (_gameType == "DotsAndBoxes" ||
        _gameType == "Morris" ||
        _gameType == "Fanorona")
    {
        gameType = _gameType;
    }
    else
    {
        gameType = "Unknown";
    }
}

void GameConfig::setHasTimeLimit(bool _hasTimeLimit)
{
    hasTimeLimit = _hasTimeLimit;
    validateAndFix();
}

void GameConfig::setTimeLimit(int _timeLimit)
{
    timeLimit = _timeLimit;
    validateAndFix();
}

void GameConfig::setServerPort(int _serverPort)
{
    serverPort = _serverPort;
    validateAndFix();
}

void GameConfig::setServerIP(const QString& _serverIP)
{
    serverIP = _serverIP.isEmpty() ? "127.0.0.1" : _serverIP;
}

void GameConfig::setDotsBoardSize(int _rows, int _columns)
{
    dotsAndBoxesRows = _rows;
    dotsAndBoxesColumns = _columns;
    validateAndFix();
}

void GameConfig::setDotsAndBoxesRows(int _dotsAndBoxesRows)
{
    dotsAndBoxesRows = _dotsAndBoxesRows;
    validateAndFix();
}

void GameConfig::setDotsAndBoxesColumns(int _dotsAndBoxesColumns)
{
    dotsAndBoxesColumns = _dotsAndBoxesColumns;
    validateAndFix();
}

void GameConfig::setUseFlyingPhase(bool _useFlyingPhase)
{
    useFlyingPhase = _useFlyingPhase;
}
