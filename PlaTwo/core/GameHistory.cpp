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

//------------------------------------ getter ------------------------------------
QStringList GameHistory::getPlayers() const
{
    return players;
}

QDateTime GameHistory::getStartTime() const
{
    return startTime;
}

QDateTime GameHistory::getEndTime() const
{
    return endTime;
}

QString GameHistory::getWinner() const
{
    return winner;
}

QString GameHistory::getGameType() const
{
    return gameType;
}

QVector<int> GameHistory::getScores() const
{
    return  scores;
}

QList<Move> GameHistory::getMoves() const
{
    return moves;
}

//------------------------------------ getter ------------------------------------
void GameHistory::setPlayers(const QStringList& _players)
{
    players = _players;
}

void GameHistory::setStartTime(const QDateTime& _startTime)
{
    startTime = _startTime;
}

void GameHistory::setEndTime(const QDateTime& _endTime)
{
    endTime = _endTime;
}

void GameHistory::setWinner(const QString& _winner)
{
    winner = _winner;
}

void GameHistory::setGameType(const QString& _gameType)
{
    gameType = _gameType;
}

void GameHistory::setScores(const QVector<int>& _scores)
{
    scores = _scores;
}

void GameHistory::setMoves(const QList<Move>& _moves)
{
    moves = _moves;
}

//------------------------------------ misc ------------------------------------
void GameHistory::addMove(const Move& move)
{
    moves.append(move);
}

//------------------------------------ working_with_JSON ------------------------------------
QJsonObject GameHistory::toJson() const
{
    QJsonObject obj;

    obj["players"] = QJsonArray::fromStringList(players);

    obj["startTime"] = startTime.toString(Qt::ISODate);
    obj["endTime"] = endTime.toString(Qt::ISODate);
    obj["duration"] = startTime.secsTo(endTime);

    obj["winner"] = winner;
    obj["gameType"] = gameType;

    QJsonArray scoresArray;
    for (int i = 0; i < scores.size(); ++i) {
        scoresArray.append(scores[i]);
    }
    obj["scores"] = scoresArray;

    QJsonArray movesArray;
    for (const auto& move : moves) {
        movesArray.append(move.toJson());
    }
    obj["moves"] = movesArray;

    return obj;
}

GameHistory GameHistory::fromJson(const QJsonObject& jsonObj)
{
    GameHistory history;

    const QJsonArray playersArray = jsonObj["players"].toArray();
    for (const auto& item : playersArray)
    {
        history.players.append(item.toString());
    }

    history.startTime = QDateTime::fromString(jsonObj["startTime"].toString(), Qt::ISODate);
    history.endTime = QDateTime::fromString(jsonObj["endTime"].toString(), Qt::ISODate);

    history.winner = jsonObj["winner"].toString();
    history.gameType = jsonObj["gameType"].toString();

    const QJsonArray scoresArray = jsonObj["scores"].toArray();
    history.scores.clear();
    for (const auto& item : scoresArray)
    {
        history.scores.append(item.toInt());
    }

    if (history.scores.size() == 1)
    {
        history.scores.append(0);
    }

    if (history.scores.isEmpty())
    {
        history.scores = QVector<int>(2, 0);
    }

    const QJsonArray movesArray = jsonObj["moves"].toArray();
    for (const auto& item : movesArray)
    {
        history.moves.append(Move::fromJson(item.toObject()));
    }

    return history;
}