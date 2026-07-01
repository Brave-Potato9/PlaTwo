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

//------------------------------------ operational_methods ------------------------------------
bool GameConfig::isValid() const
{
    if (gameType.isEmpty() || gameType == "Unknown")
    {
        return false;
    }

    if (hasTimeLimit && timeLimit <= 0)
    {
        return false;
    }

    if (serverPort < 1024 || serverPort > 65535)
    {
        return false;
    }

    if (gameType == "DotsAndBoxes")
    {
        if (dotsAndBoxesRows < 2 || dotsAndBoxesColumns < 2)
        {
            return false;
        }
    }

    return true;
}

QString GameConfig::toString() const
{
    QString str = QString("GameConfig: [Type=%1, TimeLimit=%2s, Port=%3]")
            .arg(gameType,
                 hasTimeLimit ? QString::number(timeLimit) : "No Limit",
                 QString::number(serverPort));

    if (gameType == "DotsAndBoxes")
    {
        str += QString(", D&B: %1x%2")
                   .arg(dotsAndBoxesRows)
                   .arg(dotsAndBoxesColumns);
    }
    else if (gameType == "Morris")
    {
        str += QString(", Flying: %1").arg(useFlyingPhase ? "Yes" : "No");
    }
    else if (gameType == "Fanorona")
    {
        str += ", Size: 5x5 (Fixed)";
    }

    return str;
}

void GameConfig::resetToDefaults()
{
    *this = GameConfig();
}

//------------------------------------ helper_methods ------------------------------------
void GameConfig::validateAndFix()
{

    if (hasTimeLimit && timeLimit < 1)
    {
        timeLimit = 1;
    }

    if (serverPort < 1024 || serverPort > 65535)
    {
        serverPort = 12345;
    }

    if (dotsAndBoxesRows < 2)
    {
        dotsAndBoxesRows = 2;
    }

    if (dotsAndBoxesColumns < 2)
    {
        dotsAndBoxesColumns = 2;
    }

    if (gameType.isEmpty())
    {
        gameType = "Unknown";
    }
}

//------------------------------------ working_with_JSON ------------------------------------
QJsonObject GameConfig::toJson() const
{
    QJsonObject obj;
    obj["gameType"] = gameType;
    obj["hasTimeLimit"] = hasTimeLimit;
    obj["timeLimit"] = timeLimit;
    obj["serverPort"] = serverPort;
    obj["serverIP"] = serverIP;
    obj["dotsAndBoxesRows"] = dotsAndBoxesRows;
    obj["dotsAndBoxesColumns"] = dotsAndBoxesColumns;
    obj["useFlyingPhase"] = useFlyingPhase;

    return obj;
}

GameConfig GameConfig::fromJson(const QJsonObject& jsonObj)
{
    GameConfig config;
    config.gameType = jsonObj["gameType"].toString("Unknown");
    config.hasTimeLimit = jsonObj["hasTimeLimit"].toBool(false);
    config.timeLimit = jsonObj["timeLimit"].toInt(60);
    config.serverPort = jsonObj["serverPort"].toInt(12345);
    config.serverIP = jsonObj["serverIP"].toString("127.0.0.1");
    config.dotsAndBoxesRows = jsonObj["dotsAndBoxesRows"].toInt(5);
    config.dotsAndBoxesColumns = jsonObj["dotsAndBoxesColumns"].toInt(5);
    config.useFlyingPhase = jsonObj["useFlyingPhase"].toBool(true);

    config.validateAndFix();

    return config;
}

//------------------------------------ operators ------------------------------------
bool GameConfig::operator==(const GameConfig& other) const
{
    return (gameType == other.gameType &&
            hasTimeLimit == other.hasTimeLimit &&
            timeLimit == other.timeLimit &&
            serverPort == other.serverPort &&
            serverIP == other.serverIP &&
            dotsAndBoxesRows == other.dotsAndBoxesRows &&
            dotsAndBoxesColumns == other.dotsAndBoxesColumns &&
            useFlyingPhase == other.useFlyingPhase);
}

bool GameConfig::operator!=(const GameConfig& other) const
{
    return !(*this == other);
}