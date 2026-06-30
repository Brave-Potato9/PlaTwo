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