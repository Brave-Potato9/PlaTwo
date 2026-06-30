#include "Move.h"

//------------------------------------ constructors ------------------------------------
Move::Move():
    moveType(Type::Unknown),
    moveNumber(0),
    isValidMove(false)
{
    generateTimestamp();
}

Move::Move(const QString& _playerUsername, Type _moveType):
    playerUsername(_playerUsername),
    moveType(_moveType),
    moveNumber(0),
    isValidMove(false)
{
    generateTimestamp();
}

Move::Move(const QString& _playerUsername, int from, int to):
    playerUsername(_playerUsername),
    moveType(Type::Movement),
    moveNumber(0),
    isValidMove(false)
{
    data["from"] = from;
    data["to"] = to;
    generateTimestamp();
}

Move::Move(const QString& _playerUsername, int row, int column, bool horizontal):
    playerUsername(_playerUsername),
    moveType(Type::Line),
    moveNumber(0),
    isValidMove(false)
{
    data["row"] = row;
    data["column"] = column;
    data["horizontal"] = horizontal;
    generateTimestamp();
}

Move::Move(const QString& _playerUsername, const QPoint& from, const QPoint& to):
    playerUsername(_playerUsername),
    moveType(Type::Movement),
    moveNumber(0),
    isValidMove(false)
{
    data["fromX"] = from.x();
    data["fromY"] = from.y();
    data["toX"] = to.x();
    data["toY"] = to.y();

    //calculate the direction
    int dx = to.x() - from.x();
    int dy = to.y() - from.y();
    data["directionX"] = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    data["directionY"] = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

    generateTimestamp();
}

Move::Move(const QString& _playerUsername, const QPoint& position):
    playerUsername(_playerUsername),
    moveType(Type::Placement),
    moveNumber(0),
    isValidMove(false)
{
    data["x"] = position.x();
    data["y"] = position.y();

    generateTimestamp();
}

//------------------------------------ getters ------------------------------------
QString Move::getPlayerUsername() const
{
    return playerUsername;
}

Move::Type Move::getMoveType() const
{
    return moveType;
}

QString Move::getTypeString() const
{
    return typeToString(moveType);
}

int Move::getMoveNumber() const
{
    return moveNumber;
}

QDateTime Move::getTimestamp() const
{
    return timestamp;
}

bool Move::getIsValidMove() const
{
    return isValidMove;
}

QString Move::getDescription() const
{
    return description;
}

//------------------------------------ setters ------------------------------------
void Move::setPlayerUsername(const QString& _playerUsername)
{
    playerUsername = _playerUsername;
}

void Move::setMoveType(Type _moveType)
{
    moveType = _moveType;
}

void Move::setMoveNumber(int _moveNumber)
{
    moveNumber = _moveNumber;
}

void Move::setTimestamp(const QDateTime& _timestamp)
{
    timestamp = _timestamp;
}

void Move::setIsValidMove(bool _isValidMoved)
{
    isValidMove = _isValidMoved;
}

void Move::setDescription(const QString& _description)
{
    description = _description;
}

//------------------------------------ data_methods ------------------------------------
QVariant Move::getData(const QString& key) const
{
    return data.value(key);
}

void Move::setData(const QString& key, const QVariant& value)
{
    data[key] = value;
}

bool Move::hasData(const QString& key) const
{
    return data.contains(key);
}

QMap<QString, QVariant> Move::getAllData() const
{
    return data;
}

//------------------------------------ dots_and_boxes_methods ------------------------------------
void Move::setDotsAndBoxesData(int row, int column, bool horizontal)
{
    moveType = Type::Line;
    data["row"] = row;
    data["column"] = column;
    data["horizontal"] = horizontal;
}

bool Move::isHorizontal() const
{
    return data.value("horizontal", false).toBool();
}

int Move::getRow() const
{
    return data.value("row", -1).toInt();
}

int Move::getColumn() const
{
    return data.value("row", -1).toInt();
}

//------------------------------------ morrises_methods ------------------------------------
void Move::setMorrisData(int from, int to, bool isRemoval)
{
    if (from == -1)
    {
        moveType = Type::Placement;
    }
    else if (isRemoval)
    {
        moveType = Type::Removal;
    }
    else
    {
        moveType = Type::Movement;
    }

    data["from"] = from;
    data["to"] = to;
    data["isRemoval"] = isRemoval;
}

int Move::getFrom() const
{
    return data.value("from", -1).toInt();
}

int Move::getTo() const
{
    return data.value("to", -1).toInt();
}

bool Move::isRemovalMove() const
{
    return data.value("isRemoval", false).toBool();
}

bool Move::isPlacementMove() const
{
    return ((moveType == Type::Placement) || (data.value("from", 0).toInt() == -1));
}