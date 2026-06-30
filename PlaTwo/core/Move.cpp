#include "Move.h"
#include <QIODevice>


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

//------------------------------------ fanarona_methods ------------------------------------
void Move::setFanoronaData(const QPoint& from, const QPoint& to)
{
    moveType = Type::Movement;
    data["fromX"] = from.x();
    data["fromY"] = from.y();
    data["toX"] = to.x();
    data["toY"] = to.y();

    int dx = to.x() - from.x();
    int dy = to.y() - from.y();
    data["directionX"] = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    data["directionY"] = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
}

QPoint Move::getFromPoint() const
{
    return QPoint(data.value("fromX", 0).toInt(), data.value("fromY", 0).toInt());
}

QPoint Move::getToPoint() const
{
    return QPoint(data.value("toX", 0).toInt(), data.value("toY", 0).toInt());
}

QPoint Move::getDirection() const
{
    return QPoint(data.value("directionX", 0).toInt(), data.value("directionY", 0).toInt());
}

//------------------------------------ working_with_JSON ------------------------------------
QJsonObject Move::toJson() const
{
    QJsonObject jsonObj;
    jsonObj["playerUsername"] = playerUsername;
    jsonObj["moveType"] = typeToString(moveType);
    jsonObj["moveNumber"] = moveNumber;
    jsonObj["timestamp"] = timestamp.toString(Qt::ISODate);
    jsonObj["description"] = description;
    jsonObj["isValidMove"] = isValidMove;

    QJsonObject dataObj;
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        if (it.value().typeId() == QMetaType::Int)
        {
            dataObj[it.key()] = it.value().toInt();
        }
        else if (it.value().typeId() == QMetaType::Bool)
        {
            dataObj[it.key()] = it.value().toBool();
        }
        else if (it.value().typeId() == QMetaType::QString)
        {
            dataObj[it.key()] = it.value().toString();
        }
        else if (it.value().typeId() == QMetaType::Double)
        {
            dataObj[it.key()] = it.value().toDouble();
        }
        else
        {
            dataObj[it.key()] = it.value().toString();
        }
    }

    jsonObj["data"] = dataObj;

    return jsonObj;
}

Move Move::fromJson(const QJsonObject& jsonObj)
{
    Move move;
    move.playerUsername = jsonObj["playerUsername"].toString();
    move.moveType = move.stringToType(jsonObj["moveType"].toString());
    move.moveNumber = jsonObj["moveNumber"].toInt();
    move.timestamp = QDateTime::fromString(jsonObj["timestamp"].toString(), Qt::ISODate);
    move.description = jsonObj["description"].toString();
    move.isValidMove = jsonObj["isValidMove"].toBool(true);

    QJsonObject dataObj = jsonObj["data"].toObject();
    for (auto it = dataObj.begin(); it != dataObj.end(); ++it)
    {
        if (it.value().isDouble())
        {
            move.data[it.key()] = it.value().toInt();
        }
        else if (it.value().isBool())
        {
            move.data[it.key()] = it.value().toBool();
        }
        else if (it.value().isString())
        {
            move.data[it.key()] = it.value().toString();
        }
    }

    return move;
}

//------------------------------------ serialization ------------------------------------
QByteArray Move::serialize() const
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << playerUsername;
    stream << static_cast<int>(moveType);
    stream << moveNumber;
    stream << timestamp;
    stream << description;
    stream << isValidMove;
    stream << data;
    return byteArray;
}

Move Move::deserialize(const QByteArray& _data)
{
    Move move;
    QDataStream stream(_data);
    int typeInt;
    stream >> move.playerUsername;
    stream >> typeInt;
    move.moveType = static_cast<Type>(typeInt);
    stream >> move.moveNumber;
    stream >> move.timestamp;
    stream >> move.description;
    stream >> move.isValidMove;
    stream >> move.data;
    return move;
}

//------------------------------------ operators ------------------------------------
bool Move::operator==(const Move& other) const
{
    return (playerUsername == other.playerUsername &&
            moveType == other.moveType &&
            moveNumber == other.moveNumber &&
            timestamp == other.timestamp &&
            description == other.description &&
            isValidMove == other.isValidMove &&
            data == other.data);
}

bool Move::operator!=(const Move& other) const
{
    return !(*this == other);
}

//------------------------------------ helper_methods ------------------------------------
void Move::generateTimestamp()
{
    timestamp = QDateTime::currentDateTime();
}

void Move::setDefaultType(Type type)
{
    moveType = type;
}

QString Move::typeToString(Type type) const
{
    switch (type) {
    case Type::Unknown:
        return "Unknown";
    case Type::Placement:
        return "Placement";
    case Type::Movement:
        return "Movement";
    case Type::Removal:
        return "Removal";
    case Type::Line:
        return "Line";
    case Type::Capture:
        return "Capture";
    case Type::Mill:
        return "Mill";
    default:
        return "Invalid";
    }
}

Move::Type Move::stringToType(const QString& _string) const
{
    if (_string == "Placement")
        return Type::Placement;
    if (_string == "Movement")
        return Type::Movement;
    if (_string == "Removal")
        return Type::Removal;
    if (_string == "Line")
        return Type::Line;
    if (_string == "Capture")
        return Type::Capture;
    if (_string == "Mill")
        return Type::Mill;
    return Type::Unknown;
}

//------------------------------------ to_string_methods ------------------------------------
QString Move::toString() const
{
    QString str = QString("Move #%1 by '%2' at %3 | Type: %4")
    .arg(QString::number(moveNumber),
         playerUsername,
         timestamp.toString("hh:mm:ss"),
         typeToString(moveType));

    if (moveType == Type::Line)
    {
        str += QString(" | Row: %1, Column: %2, Horizontal: %3")
        .arg(getRow())
            .arg(getColumn())
            .arg(isHorizontal() ? "Yes" : "No");
    }
    else if (moveType == Type::Movement || moveType == Type::Placement)
    {
        str += QString(" | From: (%1, %2), To: (%3, %4)")
        .arg(getFromPoint().x())
            .arg(getFromPoint().y())
            .arg(getToPoint().x())
            .arg(getToPoint().y());
    }
    else if (moveType == Type::Removal)
    {
        str += QString(" | Remove piece at: %1").arg(getTo());
    }
    else if (moveType == Type::Capture)
    {
        str += QString(" | Capture at: (%1, %2)")
        .arg(getData("captureX").toInt())
            .arg(getData("captureY").toInt());
    }

    if (!description.isEmpty())
    {
        str += " | Description: " + description;
    }

    if (!isValidMove)
    {
        str += " [INVALID MOVE]";
    }

    return str;
}

QString Move::toShortString() const
{
    QString str = playerUsername + ": ";

    switch (moveType)
    {
    case Type::Line:
        str += QString("Line (%1, %2) %3")
                   .arg(getRow())
                   .arg(getColumn())
                   .arg(isHorizontal() ? "H" : "V");
        break;
    case Type::Movement:
        str += QString("%1 -> %2")
                   .arg(getFrom())
                   .arg(getTo());
        break;
    case Type::Placement:
        str += QString("Place at %1").arg(getTo());
        break;
    case Type::Removal:
        str += QString("Remove %1").arg(getTo());
        break;
    case Type::Capture:
        str += QString("Capture");
        break;
    case Type::Mill:
        str += QString("Mill!");
        break;
    default:
        str += "Unknown";
        break;
    }

    return str;
}